#pragma once

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
class Cache;

class SOQUTE_CORESHARED_EXPORT LoadMetadata : public QObject
{
	Q_OBJECT
public:
	LoadMetadata(PackageList *packages, QObject *parent = 0);

	void setRepositoryList(const QList<QUrl> &urls);

	enum Message {
		FetchingPackages,
		NetworkError,
		ParserError,
		OtherError
	};
	static QString messageToString(const Message msg, const QVariant &data);
	static QString messageToString(const QPair<Message, QVariant> &msg) { return messageToString(msg.first, msg.second); }
	bool hasMessage() const
	{
		return !m_messages.isEmpty();
	}
	QPair<Message, QVariant> takeLastMessage()
	{
		return m_messages.dequeue();
	}

	void loadMetadata(bool refreshAll = false);

	bool isDone() const
	{
		return m_isDone;
	}
	bool isSuccess() const
	{
		return m_isSuccess;
	}

private:
	PackageList *m_packages;
	QNetworkAccessManager *m_manager;
	Cache *m_cache;
	QQueue<QPair<Message, QVariant>> m_messages;
	bool m_isDone;
	bool m_isSuccess;

	/// List of all URLs that should be downloaded
	QList<QUrl> m_urls;

	/// The number of urls that are going to be fetched. Used to determine when we are done.
	int m_numRequests;

	void addMessage(const Message msg, const QVariant &data = QVariant());
	void finish();

private
slots:
	void networkFinished();
	void networkProgress(qint64 received, qint64 total);
	void networkError();
	void parseError(const QString &errorString, const int offset);

signals:
	void message(const Message msg, const QVariant &data);
	void done(const bool success);
};
