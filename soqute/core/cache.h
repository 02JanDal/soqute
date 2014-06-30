#pragma once

#include <QString>
#include <QDir>
#include <QUrl>
#include <QDateTime>

class QNetworkRequest;
class QNetworkReply;

/**
 * \brief The Cache class encapsulates a disk cache
 *
 * \throws Most methods in this class including the constructor can throw exceptions, so make
 * sure to always catch exceptions when using this class
 */
class Cache
{
public:
	explicit Cache(const QDir &cacheDir);

	/// removes all cache entries using \see removeEntry that don't exist in \p in
	void cleanCache(const QList<QUrl> &in);

	/** creates network requests for all given urls if \p refreshAll is true, otherwise only for
	 *  those who are stale
	 */
	QPair<QList<QNetworkRequest>, QStringList> createRequests(const QList<QUrl> &in,
															  bool refreshAll);

	/// saves the data to disk and either creates or updates a cache entry
	void addEntry(const QUrl &url, const QByteArray &data);

	///	overload of addEntry. returns the name of the file containing the data, possibly updated
	QString addEntry(QNetworkReply *reply);

private:
	struct Entry
	{
		QByteArray md5;
		QUrl url;
		QString filename;
		QDateTime updated;

		bool operator==(const Entry &other)
		{
			return md5 == other.md5 && url == other.url && filename == other.filename &&
				   updated == other.updated;
		}
	};

	QDir m_dir;
	QString m_cacheMetadataFilename;
	QList<Entry> m_entries;

	/// removes all items in \p that exist in the cache and still are valid
	QList<QUrl> getStaleUrls(const QList<QUrl> &in);

	/// removes an entry from the cache, both from the cache metadata and from disk
	void removeEntry(const Entry &entry);

	QList<Entry>::const_iterator findEntry(const QUrl &url) const;
	void setEntry(const Entry &entry);
	Entry getEntry(const QUrl &url) const;

	QString absoluteFilename(const Entry &entry) const
	{
		return m_dir.absoluteFilePath(entry.filename);
	}

	/// reads \see m_entries from disk
	void readCacheFile();
	/// writes \see m_entries to disk
	void writeCacheFile();
};
