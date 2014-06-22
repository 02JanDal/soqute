#include "filesystem.h"

#include <QDir>
#include <QSaveFile>
#include <QFileInfo>

void FS::ensureExists(const QDir &dir)
{
	if (!QDir().mkpath(dir.absolutePath())) {
		throw FileSystemException("Unable to create directory " + dir.dirName() + " (" +
								  dir.absolutePath() + ")");
	}
}

void FS::remove(const QString &filename)
{
	QFile file(filename);
	if (!file.remove()) {
		throw FileSystemException("Unable to remove " + filename + ": " + file.errorString());
	}
}
void FS::remove(const QDir &dir)
{
	if (!QDir(dir).removeRecursively()) {
		throw FileSystemException("Unable to remove " + dir.dirName());
	}
}
void FS::remove(const QFileInfo &info)
{
	if (info.isDir()) {
		remove(QDir(info.absoluteFilePath()));
	} else if (info.isFile()) {
		remove(info.absoluteFilePath());
	}
}

void FS::write(const QString &filename, const QByteArray &data)
{
	QSaveFile file(filename);
	if (!file.open(QSaveFile::WriteOnly)) {
		throw FileSystemException("Couldn't open " + filename + " for writing: " +
								  file.errorString());
	}
	if (data.size() != file.write(data)) {
		throw FileSystemException("Error writing data to " + filename + ": " +
								  file.errorString());
	}
	if (!file.commit()) {
		throw FileSystemException("Error while committing data to " + filename + ": " +
								  file.errorString());
	}
}

QByteArray FS::read(const QString &filename)
{
	QFile file(filename);
	if (!file.open(QFile::ReadOnly)) {
		throw FileSystemException("Unable to open " + filename + " for reading: " +
								  file.errorString());
	}
	const qint64 size = file.size();
	QByteArray data(size, 0);
	const qint64 ret = file.read(data.data(), size);
	if (ret == -1 || ret != size) {
		throw FileSystemException("Error reading data from " + filename + ": " +
								  file.errorString());
	}
	return data;
}
