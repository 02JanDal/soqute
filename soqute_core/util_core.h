#ifndef UTIL_H
#define UTIL_H

class Package;
class PackageList;
template<typename T> class QList;
class QStringList;
class QString;
class QDir;

class KArchiveEntry;

namespace Util
{

/**
 * \returns The default package manager for this platform
 */
QString defaultPackageManager();

/**
 * \returns The platform string of this platform
 * Examples: linux-g++-64, linux-g++-32, macx-g++, win32-msvc2010
 */
QString currentPlatform();

/**
 * \param developerMode If true alpha, beta and rc versions will be matched higher than a stable version
 * \returns true if v1 is higher than v2
 */
bool isVersionHigherThan(const QString& v1, const QString& v2, const bool developerMode = false);

/**
 * Cleans the list of packages by removing any duplicates and only keeping the highest version of each id/platform combination
 * \returns A cleaned list of packages
 */
QList<const Package*> cleanPackagePointerList(const QList<const Package*> packages);

QString installerProgramForPackageManager(const QString& manager);

bool stringListToPackageList(PackageList* packages, const QStringList &packagesIn, QList<const Package*>& packagesOut, QStringList& alreadyInstalledPackagesOut, QString* notFoundPackage = 0);

QString installationRoot(const QString& version, const QString& platform);
QDir removalScriptsDirectory();

void ensureExists(const QString& directory);
void removeDirectoryRecursive(QDir directory);
void removeEmptyRecursive(const QDir& dir);

void mergeDirectoryInto(const QDir& source, const QDir& destination);
void installArchiveEntry(const KArchiveEntry* entry, const QString& destination);

template<typename T>
QList<T> removeDuplicatesFromList(const QList<T>& in);

}

#endif // UTIL_H
