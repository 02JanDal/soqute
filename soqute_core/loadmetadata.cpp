#include "loadmetadata.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "package.h"

LoadMetadata::LoadMetadata(PackageList* packages, QObject *parent) :
	QObject(parent), m_packages(packages)
{
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(networkFinished(QNetworkReply*)));

	connect(m_packages, SIGNAL(parseError(QString,int)), this, SLOT(parseError(QString,int)));
}

void LoadMetadata::setRepositoryList(const QList<QUrl> &urls)
{
	m_urls = urls;
}

QString LoadMetadata::messageToString(const LoadMetadata::Message msg, const QVariant &data)
{
	switch (msg) {
	case FetchingPackages: {
		return tr("Fetching metadata...");
	}
	case NetworkError: {
		QMap<QString, QVariant> d = data.toMap();
		return tr("Network error receiving %1: %2").arg(d["url"].toString(), d["errorString"].toString());
	}
	case ParserError: {
		QMap<QString, QVariant> d = data.toMap();
		return tr("Error parsing metadata: %1").arg(d["errorString"].toString());
	}
	default:
		return QString();
	}
}

void LoadMetadata::downloadMetadata()
{
	addMessage(FetchingPackages);
    m_numRequests = m_urls.size();
    foreach (const QUrl& url, m_urls) {
        QNetworkRequest request(url);
        QNetworkReply* reply = m_manager->get(request);
        connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(progress(qint64,qint64)));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error()));
    }
}

void LoadMetadata::networkFinished(QNetworkReply *reply)
{
	if (reply->error() != QNetworkReply::NoError) {
		QMap<QString, QVariant> data;
		data["url"] = reply->url();
		data["error"] = reply->error();
		data["errorString"] = reply->errorString();
		addMessage(NetworkError, data);
    }

    if (m_packages->parse(reply->readAll())) {
        m_numRequests--;
		if (m_numRequests == 0) {
			finish(true);
        }
    }
}

void LoadMetadata::progress(qint64 received, qint64 total)
{
    Q_UNUSED(received);
    Q_UNUSED(total);
    // TODO output progress
}

void LoadMetadata::error()
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	QMap<QString, QVariant> data;
	data["url"] = reply->url();
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

void LoadMetadata::addMessage(const LoadMetadata::Message msg, const QVariant &data)
{
	m_messages.enqueue(qMakePair(msg, data));
	emit message(msg, data);
}

void LoadMetadata::finish(bool success)
{
	m_isDone = true;
	m_isSuccess = success;
	emit done(m_isSuccess);
}
