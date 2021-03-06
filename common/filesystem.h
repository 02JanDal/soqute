#pragma once

#include "exception.h"

class QDir;
class QFileInfo;

namespace FS
{
DECLARE_EXCEPTION(FileSystem);

void ensureExists(const QDir &dir);
void remove(const QString &filename);
void remove(const QDir &dir);
void remove(const QFileInfo &info);
void copy(const QString &from, const QString &to);
void move(const QString &from, const QString &to);
void write(const QString &filename, const QByteArray &data);
bool exists(const QString &filename);
bool exists(const QDir &directory);
QByteArray read(const QString &filename);

void removeEmptyRecursive(const QDir &dir);
void mergeDirectoryInto(const QDir &source, const QDir &destination);
}
