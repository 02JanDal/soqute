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
void write(const QString &filename, const QByteArray &data);
QByteArray read(const QString &filename);
}
