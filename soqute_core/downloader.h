#pragma once

#include "actor.h"

#include <QUrl>
#include <QDir>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
class QAuthenticator;
class QSslError;
class QDir;
QT_END_NAMESPACE

class AbstractInstaller;
class PackageList;
class Package;
typedef const Package *PackagePointer;
typedef QList<PackagePointer> PackagePointerList;

class AbstractAuthenticator
{
public:
	virtual bool authenticate(QNetworkReply *reply, QAuthenticator *authenticator) = 0;
};

class Downloader : public Actor
{
	Q_OBJECT
public:
	explicit Downloader(PackageList *packages, PackagePointerList packagesToInstall,
						QObject *parent = 0);

	void setAuthenticator(AbstractAuthenticator *authenticator);

	enum MessageType {
		None,
		NothingToDo,
		FetchingPackages,
		ReceivedArchive,
		NetworkDone,
		Installing,
		Installed,
		ArchiveSaveError = messageTypeErrorOffset,
		NetworkError,
		InstallError,
		SslError,
		AuthError
	};

	void downloadAndInstall();

private:
	QNetworkAccessManager *m_manager;
	AbstractInstaller *m_installer;
	PackageList *m_packages;
	PackagePointerList m_packagesToDownload;
	AbstractAuthenticator *m_authenticator;

	QMap<PackagePointer, QString> m_packagesToInstall;
	QList<QNetworkReply *> m_unarrivedReplies;

	QDir m_baseArchivePath;

	void cleanup();
	void installNextPackage();
	bool saveToFile(const QByteArray &data, const QString &fileName, QString *errorString = 0);
	void abortNetworkRequests();

	QString messageToStringImpl(const int msg, const QVariant &data) const override;

private
slots:
	void networkDone(QNetworkReply *reply);
	void authenticationNeeded(QNetworkReply *reply, QAuthenticator *authenticator);
	void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

	// for the AbstractInstaller
	void installPackageBegin(const Package *package);
	void installPackageEnd(const Package *package);
	void errorInstalling(const QString &msg);

signals:
	void installPackage(const Package *package, const QString &fileName);
};
