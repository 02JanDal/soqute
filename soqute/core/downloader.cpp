#include "downloader.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QCoreApplication>
#include <QFile>
#include <QCryptographicHash>
#include <QSslError>
#include <QStandardPaths>

#include "package.h"
#include "jsinstaller.h"
#include "configurationhandler.h"
#include "installedpackages.h"
#include "util_core.h"

Q_DECLARE_METATYPE(QSslError)

Downloader::Downloader(PackageList *packages, PackagePointerList packagesToInstall,
					   QObject *parent)
	: Actor(parent), m_packages(packages), m_packagesToDownload(packagesToInstall),
	  m_authenticator(0)
{
	m_manager = new QNetworkAccessManager(this);
	connect(m_manager, &QNetworkAccessManager::finished, this,
			&Downloader::networkDone);
	connect(m_manager, &QNetworkAccessManager::authenticationRequired, this,
			&Downloader::authenticationNeeded);
	connect(m_manager, &QNetworkAccessManager::sslErrors, this,
			&Downloader::sslErrors);

	m_installer = new JSInstaller;
	connect(m_installer, &AbstractInstaller::installPackageBegin, this,
			&Downloader::installPackageBegin);
	connect(m_installer, &AbstractInstaller::installPackageEnd, this,
			&Downloader::installPackageEnd);
	connect(m_installer, &AbstractInstaller::error, this, &Downloader::errorInstalling,
			Qt::DirectConnection);
	connect(m_installer, &AbstractInstaller::finished, this, &Downloader::finish);
	connect(this, &Downloader::installPackage, m_installer,
			&AbstractInstaller::install, Qt::QueuedConnection);
	m_installer->start();

	m_baseArchivePath =
		QDir(QStandardPaths::standardLocations(QStandardPaths::TempLocation).first());
	m_baseArchivePath.mkpath("soqute/archives");
	m_baseArchivePath.cd("soqute");
	m_baseArchivePath.cd("archives");
}

void Downloader::setAuthenticator(AbstractAuthenticator *authenticator)
{
	m_authenticator = authenticator;
}

QString Downloader::messageToStringImpl(const int msg, const QVariant &data) const
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
		return tr("Received archive for package %1")
			.arg(Util::createFriendlyName(pkg));
	}
	case ArchiveSaveError: {
		return tr("There was an error saving the received archive: %1").arg(data.toString());
	}
	case NetworkDone: {
		return tr("All packages have been downloaded");
	}
	case NetworkError: {
		QMap<QString, QVariant> d = data.toMap();
		return tr("Network error receiving %1: %2")
			.arg(d["url"].toString(), d["errorString"].toString());
	}
	case Installing: {
		PackagePointer pkg = data.value<PackagePointer>();
		return tr("Installing %1...").arg(Util::createFriendlyName(pkg));
	}
	case Installed: {
		PackagePointer pkg = data.value<PackagePointer>();
		return tr("Installed %1").arg(Util::createFriendlyName(pkg));
	}
	case InstallError: {
		return tr("Installation error: %1").arg(data.toString());
	}
	case AuthError: {
		return data.toString();
	}
	default:
		return QString();
	}
}

void Downloader::downloadAndInstall()
{
	// reset
	setIsSuccess(true);

	for (PackagePointer package : m_packagesToDownload) {
		if (package->url().isEmpty()) {
			m_packagesToDownload.removeAll(package);
		}
	}

	if (m_packagesToDownload.isEmpty()) {
		addMessage(NothingToDo);
		return;
	}

	addMessage(FetchingPackages);
	for (PackagePointer package : m_packagesToDownload) {
		QNetworkRequest request(package->url());
		request.setHeader(QNetworkRequest::UserAgentHeader,
						  QString("%1 version %2 (QNetworkAccessManager)")
							  .arg(qApp->applicationName(), qApp->applicationVersion()));
		QNetworkReply *reply = m_manager->get(request);
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
	m_unarrivedReplies.removeAll(reply);
	if (reply->error() == QNetworkReply::NoError) {
		PackagePointer package = reply->property("package").value<PackagePointer>();
		addMessage(ReceivedArchive, QVariant::fromValue(package));
		const QString path = reply->url().path(QUrl::FullyEncoded);
		const QString fileName =
			m_baseArchivePath.filePath(path.mid(path.lastIndexOf('/') + 1));
		QString errorString;
		bool res = saveToFile(reply->readAll(), fileName, &errorString);
		if (!res) {
			addMessage(ArchiveSaveError, errorString);
			abortNetworkRequests();
		} else {
			m_packagesToInstall.insert(package, fileName);
		}
	} else {
		QMap<QString, QVariant> data;
		data["url"] = reply->url();
		data["error"] = reply->error();
		data["errorString"] = reply->errorString();
		addMessage(NetworkError, data);
	}

	if (m_unarrivedReplies.isEmpty()) {
		if (isSuccess()) {
			addMessage(NetworkDone);
			installNextPackage();
		} else {
			finish();
		}
	}
}
void Downloader::authenticationNeeded(QNetworkReply *reply, QAuthenticator *authenticator)
{
	if (m_authenticator) {
		if (!m_authenticator->authenticate(reply, authenticator)) {
			addMessage(AuthError, tr("Error while authenticating"));
		}
	}
}
void Downloader::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
	QMap<QString, QVariant> data;
	data["url"] = reply->url();
	data["errors"] = QVariant::fromValue(errors);
	addMessage(SslError, data);
}

void Downloader::installPackageBegin(const Package *package)
{
	addMessage(Installing, QVariant::fromValue(package));
}
void Downloader::installPackageEnd(const Package *package)
{
	addMessage(Installed, QVariant::fromValue(package));
	ConfigurationHandler::instance()->installedPackages()->setPackageInstalled(package);
	if (!m_packagesToDownload.isEmpty()) {
		installNextPackage();
	} else {
		m_installer->quit();
		m_installer->wait();
		finish();
	}
}

void Downloader::errorInstalling(const QString &msg)
{
	addMessage(InstallError, msg);
}

bool Downloader::saveToFile(const QByteArray &data, const QString &fileName,
							QString *errorString)
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
	for (QNetworkReply *reply : m_unarrivedReplies) {
		reply->abort();
		reply->deleteLater();
	}
}

void Downloader::installNextPackage()
{
	PackagePointer package = m_packagesToDownload.takeFirst();
	emit installPackage(package, m_packagesToInstall[package]);
}
