#include "downloader.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QCoreApplication>
#include <QFile>
#include <QCryptographicHash>
#include <QtConcurrent/QtConcurrent>
#include <QSslError>

#include "package.h"
#include "jsinstaller.h"
#include "configurationhandler.h"
#include "installedpackages.h"

Q_DECLARE_METATYPE(QSslError)

Downloader::Downloader(PackageList* packages, PackagePointerList packagesToInstall, QObject *parent) :
	QObject(parent), m_packages(packages), m_packagesToDownload(packagesToInstall), m_authenticator(0), m_isDone(false), m_isSuccess(false)
{
	m_manager = new QNetworkAccessManager(this);
	connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(networkDone(QNetworkReply*)));
	connect(m_manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(authenticationNeeded(QNetworkReply*,QAuthenticator*)));
	connect(m_manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));

	m_installer = new JSInstaller;
	connect(m_installer, SIGNAL(installPackageBegin(const Package*)), this, SLOT(installPackageBegin(const Package*)));
	connect(m_installer, SIGNAL(installPackageEnd(const Package*)), this, SLOT(installPackageEnd(const Package*)));
	connect(m_installer, SIGNAL(error(QString)), this, SLOT(errorInstalling(QString)), Qt::DirectConnection);
	connect(m_installer, SIGNAL(finished()), this, SLOT(finish()));
	connect(this, SIGNAL(installPackage(const Package*,QString)), m_installer, SLOT(install(const Package*,QString)), Qt::QueuedConnection);
	m_installer->start();

	m_baseArchivePath = QDir(QStandardPaths::standardLocations(QStandardPaths::TempLocation).first());
	m_baseArchivePath.mkpath("soqute/archives");
	m_baseArchivePath.cd("soqute");
	m_baseArchivePath.cd("archives");
}

void Downloader::setAuthenticator(AbstractAuthenticator *authenticator)
{
	m_authenticator = authenticator;
}

QString Downloader::messageToString(const Downloader::Message msg, const QVariant& data)
{
	switch (msg) {
	case None: {
		return QString();
	}
	case NothingToDo: {
        return tr("No packages to download");
	}
	case FetchingPackages: {
		return tr("Fetching packages...");
	}
	case ReceivedArchive: {
		PackagePointer pkg = data.value<PackagePointer>();
		return tr("Received archive for package %1 v%2 (%3)").arg(pkg->id(), pkg->version(), pkg->platform());
	}
	case ArchiveSaveError: {
		return tr("There was an error saving the received archive: %1").arg(data.toString());
	}
	case NetworkDone: {
		return tr("All packages have been downloaded");
	}
	case NetworkError: {
		QMap<QString, QVariant> d = data.toMap();
		return tr("Network error receiving %1: %2").arg(d["url"].toString(), d["errorString"].toString());
	}
	case Installing: {
		PackagePointer pkg = data.value<PackagePointer>();
		return tr("Installing %1 v%2 (%3)...").arg(pkg->id(), pkg->version(), pkg->platform());
	}
	case Installed: {
		PackagePointer pkg = data.value<PackagePointer>();
		return tr("Installed %1 v%2 (%3)").arg(pkg->id(), pkg->version(), pkg->platform());
	}
	case InstallError: {
		return tr("Installation error: %1").arg(data.toString());
	}
	default:
		return QString();
	}
}

void Downloader::downloadAndInstall()
{
    foreach (PackagePointer package, m_packagesToDownload) {
		if (package->url().isEmpty()) {
            m_packagesToDownload.removeAll(package);
		}
	}

    if (m_packagesToDownload.isEmpty()) {
		addMessage(NothingToDo);
		return;
	}

	addMessage(FetchingPackages);
    foreach (PackagePointer package, m_packagesToDownload) {
		QNetworkRequest request(package->url());
		request.setHeader(QNetworkRequest::UserAgentHeader, QString("%1 version %2 (QNetworkAccessManager)").arg(qApp->applicationName(), qApp->applicationVersion()));
		QNetworkReply* reply = m_manager->get(request);
		reply->setProperty("package", QVariant::fromValue(package));
		m_unarrivedReplies.append(reply);
	}
}
void Downloader::cleanup()
{
	m_baseArchivePath.removeRecursively();
}

void Downloader::networkDone(QNetworkReply *reply)
{
	if (reply->error() == QNetworkReply::NoError) {
		m_unarrivedReplies.removeAll(reply);
		PackagePointer package = reply->property("package").value<PackagePointer>();
		addMessage(ReceivedArchive, QVariant::fromValue(package));
		const QString path = reply->url().path(QUrl::FullyEncoded);
		const QString fileName = m_baseArchivePath.filePath(path.right(path.lastIndexOf('/')));
		QString errorString;
		bool res = saveToFile(reply->readAll(), fileName, &errorString);
		if (!res) {
			addMessage(ArchiveSaveError, errorString);
			abortNetworkRequests();
			finish(false);
			return;
		}
        m_packagesToInstall.insert(package, fileName);

		if (m_unarrivedReplies.isEmpty()) {
			addMessage(NetworkDone);
			installNextPackage();
		}
	} else {
		QMap<QString, QVariant> data;
		data["url"] = reply->url();
		data["error"] = reply->error();
		data["errorString"] = reply->errorString();
		addMessage(NetworkError, data);
		finish(false);
		return;
	}
}
void Downloader::authenticationNeeded(QNetworkReply *reply, QAuthenticator *authenticator)
{
	if (m_authenticator) {
		m_authenticator->authenticate(reply, authenticator);
	}
}
void Downloader::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
	QMap<QString, QVariant> data;
	data["url"] = reply->url();
	data["errors"] = QVariant::fromValue(errors);
	addMessage(SslError, data);
	finish(false);
}

void Downloader::installPackageBegin(const Package *package)
{
	addMessage(Installing, QVariant::fromValue(package));
}
void Downloader::installPackageEnd(const Package *package)
{
	addMessage(Installed, QVariant::fromValue(package));
	ConfigurationHandler::instance()->installedPackages()->setPackageInstalled(package->id(), package->version(), package->platform());
    if (!m_packagesToDownload.isEmpty()) {
		installNextPackage();
	} else {
		m_installer->quit();
		m_installer->wait();
		finish(true);
	}
}

void Downloader::errorInstalling(const QString &msg)
{
	addMessage(InstallError, msg);
	finish(false);
}

bool Downloader::saveToFile(const QByteArray &data, const QString& fileName, QString* errorString)
{
	QFile f(fileName);
	if (f.open(QFile::WriteOnly | QFile::Truncate)) {
		f.write(data);
		f.close();
		return true;
	}
	if (errorString) {
		*errorString = f.errorString();
	}
	return false;
}

void Downloader::abortNetworkRequests()
{
	foreach (QNetworkReply* reply, m_unarrivedReplies) {
		reply->abort();
		reply->deleteLater();
	}
}

void Downloader::addMessage(const Downloader::Message msg, const QVariant &data)
{
	m_messages.enqueue(qMakePair(msg, data));
	emit message(msg, data);
}

void Downloader::finish(bool success)
{
	m_isDone = true;
	m_isSuccess = success;
	emit done(m_isSuccess);
}

void Downloader::installNextPackage()
{
    PackagePointer package = m_packagesToDownload.takeFirst();
    emit installPackage(package, m_packagesToInstall[package]);
}
