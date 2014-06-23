#pragma once

template <typename T> class QList;
class Package;
class PackageList;
typedef const Package *PackagePointer;
typedef QList<PackagePointer> PackagePointerList;
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
 * \param developerMode If true alpha, beta and rc versions will be matched higher than a stable
 * version
 * \returns true if v1 is higher than v2
 */
bool isVersionHigherThan(const QString &v1, const QString &v2,
						 const bool developerMode = false);

/**
 * Cleans the list of packages by removing any duplicates and only keeping the highest version
 * of each id/platform combination
 * \returns A cleaned list of packages
 */
QList<PackagePointer> cleanPackagePointerList(const QList<PackagePointer> packages);

QString createFriendlyName(PackagePointer package);

QString installerProgramForPackageManager(const QString &manager);

bool stringListToPackageList(PackageList *packages, const QStringList &packagesIn,
							 QList<const Package *> &packagesOut, const QString &host,
							 QStringList *alreadyInstalledPackagesOut,
							 QString *notFoundPackage = 0);

QDir installationRoot(const QString &version, const QString &host, const QString &target);
QDir removalScriptsDirectory();

void installArchiveEntry(const KArchiveEntry *entry, const QString &destination);

template <typename T> QList<T> removeDuplicatesFromList(const QList<T> &in);
PackagePointerList removeDuplicatesFromList(const PackagePointerList &in);
}
