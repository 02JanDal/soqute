#pragma once

#include <QObject>

#include <QQueue>
#include <QUrl>
#include <QDir>
#include <QVariant>

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
	virtual void authenticate(QNetworkReply *reply, QAuthenticator *authenticator) = 0;
};

class Downloader : public QObject
{
	Q_OBJECT
public:
	explicit Downloader(PackageList *packages, PackagePointerList packagesToInstall,
						QObject *parent = 0);

	void setAuthenticator(AbstractAuthenticator *authenticator);

	enum Message {
		None,
		NothingToDo,
		FetchingPackages,
		ReceivedArchive,
		ArchiveSaveError,
		NetworkDone,
		NetworkError,
		Installing,
		Installed,
		InstallError,
		SslError
	};
	static QString messageToString(const Message msg, const QVariant &data);
	bool hasMessage() const
	{
		return !m_messages.isEmpty();
	}
	QPair<Message, QVariant> takeLastMessage()
	{
		return m_messages.dequeue();
	}

	void downloadAndInstall();

	bool isDone() const
	{
		return m_isDone;
	}
	bool isSuccess() const
	{
		return m_isSuccess;
	}

private:
	QNetworkAccessManager *m_manager;
	AbstractInstaller *m_installer;
	PackageList *m_packages;
	PackagePointerList m_packagesToDownload;
	AbstractAuthenticator *m_authenticator;
	QQueue<QPair<Message, QVariant>> m_messages;
	bool m_isDone;
	bool m_isSuccess;

	QMap<PackagePointer, QString> m_packagesToInstall;
	QList<QNetworkReply *> m_unarrivedReplies;

	QDir m_baseArchivePath;

	void cleanup();
	void installNextPackage();
	bool saveToFile(const QByteArray &data, const QString &fileName, QString *errorString = 0);
	void abortNetworkRequests();

	void addMessage(const Message msg, const QVariant &data = QVariant());

private
slots:
	void networkDone(QNetworkReply *reply);
	void authenticationNeeded(QNetworkReply *reply, QAuthenticator *authenticator);
	void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

	void finish(bool success = true);

	// for the AbstractInstaller
	void installPackageBegin(const Package *package);
	void installPackageEnd(const Package *package);
	void errorInstalling(const QString &msg);

signals:
	void installPackage(const Package *package, const QString &fileName);
	void message(const Message msg, const QVariant &data);
	void done(const bool success);
};
