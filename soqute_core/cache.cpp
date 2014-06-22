#include "cache.h"

#include <QCryptographicHash>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "json.h"
#include "filesystem.h"

Cache::Cache(const QDir &cacheDir) : m_dir(cacheDir)
{
	m_cacheMetadataFilename = m_dir.absoluteFilePath(".cache.json");
	FS::ensureExists(m_dir);
	m_dir.refresh();
	if (m_dir.exists(m_cacheMetadataFilename)) {
		readCacheFile();
	} else {
		Json::write(QJsonArray(), m_cacheMetadataFilename); // ensure that there's a file
															// available for reading
	}
}

void Cache::cleanCache(const QList<QUrl> &in)
{
	// purge entries that don't exist in 'in'
	{
		QList<Entry> entriesToRemove;
		for (const auto entry : m_entries) {
			if (!in.contains(entry.url)) {
				// this entry doesn't exist anymore, so remove it
				entriesToRemove += entry;
			}
		}
		for (auto entry : entriesToRemove) {
			removeEntry(entry);
		}
	}
	// remove directories, as that's not something we put there
	for (const QFileInfo &info : m_dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
		FS::remove(info);
	}
	// remove files that we don't know anything of
	for (const QFileInfo &info : m_dir.entryInfoList(QStringList() << "*.cache", QDir::Files)) {
		if (info.fileName() == ".cache.json") {
			continue;
		}
		bool found = false;
		for (const auto entry : m_entries) {
			if (absoluteFilename(entry) == info.absoluteFilePath()) {
				found = true;
				break;
			}
		}
		// it doesn't exist in the cache metadata, so we remove it
		if (!found) {
			FS::remove(info);
		}
	}
	// remove cache entries where the file isn't existing anymore
	{
		QMutableListIterator<Entry> it(m_entries);
		while (it.hasNext()) {
			if (!QFileInfo(absoluteFilename(it.next())).exists()) {
				it.remove();
			}
		}
	}
	writeCacheFile();
}

QList<QUrl> Cache::getStaleUrls(const QList<QUrl> &in)
{
	const QDateTime now = QDateTime::currentDateTime();
	QList<QUrl> out = in;
	for (auto entry : m_entries) {
		// QUESTION is one day good? maybe increase to one week?
		QDateTime lastUpdate = QDateTime(entry.updated).addDays(1);
		if (lastUpdate >= now) {
			// still valid
			out.removeAll(entry.url);
		}
	}
	return out;
}

QPair<QList<QNetworkRequest>, QStringList> Cache::createRequests(const QList<QUrl> &in,
																 bool refreshAll)
{
	QList<QUrl> urlsIn = in;
	QList<QNetworkRequest> requests;
	{
		QList<QUrl> urls = refreshAll ? in : getStaleUrls(in);
		requests.reserve(urls.size());
		for (const auto url : urls) {
			urlsIn.removeAll(url);
			QNetworkRequest request(url);
			const auto entryIt = findEntry(url);
			if (entryIt != m_entries.end()) {
				const auto entry = *entryIt;
				// TODO add the md5 as a header
			}
			requests.append(request);
		}
	}
	QStringList localfiles;
	{
		// loop through all urls that didn't get made into network requests
		for (const auto url : urlsIn) {
			localfiles.append(absoluteFilename(*findEntry(url)));
		}
	}
	return qMakePair(requests, localfiles);
}

void Cache::removeEntry(const Entry &entry)
{
	FS::remove(absoluteFilename(entry));
	m_entries.removeAll(entry);
}
void Cache::addEntry(const QUrl &url, const QByteArray &data)
{
	Entry entry = getEntry(url);
	entry.md5 = QCryptographicHash::hash(data, QCryptographicHash::Md5);
	entry.updated = QDateTime::currentDateTime();

	FS::write(absoluteFilename(entry), data);

	setEntry(entry);
}
QString Cache::addEntry(QNetworkReply *reply)
{
	const QUrl url = reply->request().url();
	// don't write to disk if the reply is empty because it's not modified
	addEntry(url, reply->readAll());

	const auto entryIt = findEntry(url);
	Q_ASSERT(entryIt != m_entries.end());
	return absoluteFilename(*entryIt);
}

QList<Cache::Entry>::const_iterator Cache::findEntry(const QUrl &url) const
{
	return std::find_if(m_entries.cbegin(), m_entries.cend(),
						[url](const Entry &entry) { return entry.url == url; });
}
void Cache::setEntry(const Entry &entry)
{
	QMutableListIterator<Entry> it(m_entries);
	while (it.hasNext()) {
		Entry e = it.next();
		if (e.url == entry.url) {
			it.setValue(entry);
			writeCacheFile();
			return;
		}
	}
	m_entries.append(entry);
	writeCacheFile();
}
Cache::Entry Cache::getEntry(const QUrl &url) const
{
	auto it = findEntry(url);
	if (it == m_entries.cend()) {
		Entry newEntry;
		newEntry.url = url;
		newEntry.filename = url.host() + url.path().replace('/', "::") + ".cache";
		return newEntry;
	}
	return *it;
}

void Cache::readCacheFile()
{
	m_entries.clear();
	const QJsonArray entries =
		Json::ensureArray(Json::ensureDocument(m_cacheMetadataFilename), "Cache root");
	for (const auto value : entries) {
		const QJsonObject object = Json::ensureObject(value, "Cache array entry");
		Entry entry;
		entry.url = Json::ensureIsType<QUrl>(object, "url");
		entry.md5 = Json::ensureIsType<QByteArray>(object, "md5");
		entry.filename = Json::ensureIsType<QString>(object, "filename");
		entry.updated = Json::ensureIsType<QDateTime>(object, "updated");
		m_entries.append(entry);
	}
}
void Cache::writeCacheFile()
{
	QJsonArray array;
	for (const auto entry : m_entries) {
		QJsonObject object;
		object["url"] = Json::toJson(entry.url);
		object["md5"] = Json::toJson(entry.md5);
		object["filename"] = entry.filename;
		object["updated"] = Json::toJson(entry.updated);
		array.append(object);
	}
	Json::write(array, m_cacheMetadataFilename);
}
