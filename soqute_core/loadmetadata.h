#ifndef LOADMETADATATASK_H
#define LOADMETADATATASK_H

#include <QObject>
#include <QQueue>
#include <QPair>
#include <QVariant>
#include <QUrl>

#include "soqute_core_global.h"

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE

class PackageList;

class SOQUTE_CORESHARED_EXPORT LoadMetadata : public QObject
{
    Q_OBJECT
public:
	LoadMetadata(PackageList *packages, QObject* parent = 0);

    void setRepositoryList(const QList<QUrl>& urls);

	enum Message {
		FetchingPackages,
		NetworkError,
		ParserError
	};
	static QString messageToString(const Message msg, const QVariant &data);
	bool hasMessage() const { return !m_messages.isEmpty(); }
	QPair<Message, QVariant> takeLastMessage() { return m_messages.dequeue(); }

	void downloadMetadata();

	bool isDone() const { return m_isDone; }
	bool isSuccess() const { return m_isSuccess; }

private:
	PackageList* m_packages;
    QNetworkAccessManager* m_manager;
	QQueue<QPair<Message, QVariant> > m_messages;
	bool m_isDone;
	bool m_isSuccess;

    /// List of all URLs that should be downloaded
    QList<QUrl> m_urls;

    /// The number of urls that are going to be fetched. Used to determine when we are done.
    int m_numRequests;

	void addMessage(const Message msg, const QVariant& data = QVariant());
	void finish(bool success = true);

private slots:
    void networkFinished(QNetworkReply* reply);
	void progress(qint64 received, qint64 total);
	void error();
	void parseError(const QString& errorString, const int offset);

signals:
	void message(const Message msg, const QVariant& data);
	void done(const bool success);
};

#endif // LOADMETADATATASK_H
