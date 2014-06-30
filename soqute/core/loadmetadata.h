#pragma once

#include "actor.h"

#include <QUrl>

#include "soqute_core_global.h"

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
QT_END_NAMESPACE

class PackageList;
class Cache;

class SOQUTE_CORESHARED_EXPORT LoadMetadata : public Actor
{
	Q_OBJECT
public:
	explicit LoadMetadata(PackageList *packages, QObject *parent = 0);

	void setRepositoryList(const QList<QUrl> &urls);

	enum MessageType {
		FetchingPackages,
		NetworkError = messageTypeErrorOffset,
		ParserError,
		OtherError
	};

	void loadMetadata(bool refreshAll = false);

private:
	PackageList *m_packages;
	QNetworkAccessManager *m_manager;
	Cache *m_cache;

	/// List of all URLs that should be downloaded
	QList<QUrl> m_urls;

	/// The number of urls that are going to be fetched. Used to determine when we are done.
	int m_numRequests;

	QString messageToStringImpl(const int msg, const QVariant &data) const override;

private
slots:
	void networkFinished();
	void networkProgress(qint64 received, qint64 total);
	void networkError();
	void parseError(const QString &errorString);
};
