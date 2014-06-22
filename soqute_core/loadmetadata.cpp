#include "loadmetadata.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkReply>
#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>

#include "configurationhandler.h"
#include "package.h"
#include "json.h"
#include "filesystem.h"
#include "cache.h"

LoadMetadata::LoadMetadata(PackageList *packages, QObject *parent)
	: Actor(parent), m_packages(packages), m_manager(new QNetworkAccessManager(this)),
	  m_cache(new Cache(
		  QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/metadata")))
{
	connect(m_packages, &PackageList::parseError, this, &LoadMetadata::parseError);
}

void LoadMetadata::setRepositoryList(const QList<QUrl> &urls)
{
	m_urls = urls;
}

QString LoadMetadata::messageToStringImpl(const int msg, const QVariant &data) const
{
	switch (msg) {
	case FetchingPackages: {
		return tr("Fetching metadata...");
	}
	case NetworkError: {
		QMap<QString, QVariant> d = data.toMap();
		return tr("Network error receiving %1: %2")
			.arg(d["url"].toString(), d["errorString"].toString());
	}
	case ParserError: {
		QMap<QString, QVariant> d = data.toMap();
		return tr("Error parsing metadata: %1").arg(d["errorString"].toString());
	}
	case OtherError: {
		return tr("Error: %1").arg(data.toString());
	}
	default:
		return QString();
	}
}

void LoadMetadata::loadMetadata(bool refreshAll)
{
	reset();
	m_numRequests = 0;
	try
	{
		m_cache->cleanCache(m_urls);
		const auto ret = m_cache->createRequests(m_urls, refreshAll);
		const auto requests = ret.first;
		const auto localfiles = ret.second;
		m_numRequests = requests.size();
		for (const auto request : requests) {
			QNetworkReply *reply = m_manager->get(request);
			connect(reply, &QNetworkReply::downloadProgress, this,
					&LoadMetadata::networkProgress);
			connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
					SLOT(networkError()));
			connect(reply, &QNetworkReply::finished, this, &LoadMetadata::networkFinished);
		}
		if (m_numRequests > 0) {
			addMessage(FetchingPackages);
		}
		for (const auto localfile : localfiles) {
			// local try-catch so we don't leave orphaned network requests
			try
			{
				m_packages->parse(FS::read(localfile));
			}
			catch (Exception &e)
			{
				addMessage(OtherError, e.message());
			}
		}
	}
	catch (Exception &e)
	{
		addMessage(OtherError, e.message());
	}

	if (m_numRequests == 0) {
		finish();
	}
}

void LoadMetadata::networkFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

	if (reply->error() == QNetworkReply::NoError) {
		try
		{
			m_packages->parse(FS::read(m_cache->addEntry(reply)));
		}
		catch (Exception &e)
		{
			addMessage(OtherError, e.message());
		}
	}

	m_numRequests--;
	if (m_numRequests == 0) {
		finish();
	}
}
void LoadMetadata::networkProgress(qint64 received, qint64 total)
{
	Q_UNUSED(received);
	Q_UNUSED(total);
	// TODO output progress
}
void LoadMetadata::networkError()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	QMap<QString, QVariant> data;
	data["url"] = reply->request().url();
	data["error"] = reply->error();
	data["errorString"] = reply->errorString();
	addMessage(NetworkError, data);
}

void LoadMetadata::parseError(const QString &errorString, const int offset)
{
	QMap<QString, QVariant> data;
	data["errorString"] = errorString;
	data["offset"] = offset;
	addMessage(ParserError, data);
}
