#include "util_core.h"

#include <QProcess>
#include <QString>
#include <QDebug>
#include <QRegularExpression>
#include <QDir>
#include <QStandardPaths>

#include "package.h"
#include "packagematcher.h"
#include "installedpackages.h"
#include "configurationhandler.h"

#include <karchive.h>

namespace Util
{

QString defaultPackageManager()
{
#if defined(Q_OS_LINUX)
    // TODO can we rely on this?
    QProcess proc;
    proc.start("lsb_release -si");
    proc.waitForStarted();
    proc.waitForFinished();
    const QString distro = QString::fromLocal8Bit(proc.readAll()).trimmed();
	QMap<QString, QString> m;
	// source: http://distrowatch.com/dwres.php?resource=package-management
	m["Ubuntu"] = "apt";
	m["openSUSE"] = "zypp";
	m["Fedora"] = "yum";
	m["CentOS"] = "yum";
	m["Mandriva"] = "urpmi";
	m["Mageia"] = "urpmi";
	m["Slackware"] = "slackpkg";
	m["Vector"] = "slapt-get";
	m["Zenwalk"] = "netpkg";
	m["Sabayon"] = "equo";
	m["Arch"] = "pacman";
	m["rPath"] = "conary";
	m["Foresight"] = "conary";
	m["Pisi"] = "pisi";
	m["SolusOS"] = "pisi";
	m["Gentoo"] = "emerge";
	m["Lunar"] = "lin";
	m["Source Mage"] = "cast";
    m["FreeBSD"] = "pkg_add";
	return m[distro];
#endif
    // TODO other operating system
    return QString();
}

QString currentPlatform()
{
#if defined(Q_OS_LINUX)
# if defined(Q_CC_GNU)
#  if defined(Q_PROCESSOR_X86_64)
    return "linux-g++-64";
#  elif defined(Q_PROCESSOR_X86_32)
    return "linux-g++-32";
#  endif
# endif
#endif
    return QString();
}

bool isVersionHigherThan(const QString& v1, const QString& v2, const bool developerMode)
{
	QMap<QString, int> titles;
	titles["alpha"] = 0;
	titles["beta"] = 1;
	titles["rc"] = 2;
	QRegularExpression exp("((?<first>\\d)*(\\.(?<second>\\d*))?(\\.(?<third>\\d*))?(\\.(?<fourth>\\d*))?)((?<title>[a-z]+)(?<titlenum>\\d*))?");
	QRegularExpressionMatch match1 = exp.match(v1);
	QRegularExpressionMatch match2 = exp.match(v2);
	const int first1 = match1.captured("first").toInt();
	const int first2 = match2.captured("first").toInt();
	const int second1 = match1.captured("second").toInt();
	const int second2 = match2.captured("second").toInt();
	const int third1 = match1.captured("third").toInt();
	const int third2 = match2.captured("third").toInt();
	const int fourth1 = match1.captured("fourth").toInt();
	const int fourth2 = match2.captured("fourth").toInt();
	const QString title1 = match1.captured("title");
	const QString title2 = match2.captured("title");
	const int titlenum1 = match1.captured("titlenum").toInt();
	const int titlenum2 = match2.captured("titlenum").toInt();
	if (first1 != first2) {
		return first1 > first2;
	} else if (second1 != second2) {
		return second1 > second2;
	} else if (third1 != third2) {
		return third1 > third2;
	} else if (fourth1 != fourth2) {
		return fourth1 > fourth2;
	} else if (!developerMode) {
		return false;
	} else {
		if (title1 != title2) {
			return titles[title1] > titles[title2];
		} else {
			return titlenum1 > titlenum2;
		}
	}
	return v1 > v2;
}

QList<const Package *> cleanPackagePointerList(const QList<const Package *> packages)
{
	QMap<QString, const Package*> out;

	foreach (const Package* package, packages) {
		const QString identifier = QString("%1#%2").arg(package->id(), package->platform());
		if (!out.contains(identifier) || isVersionHigherThan(package->version(), out[identifier]->version())) {
			out.insert(identifier, package);
		}
	}

	return out.values();
}

QString installerProgramForPackageManager(const QString &manager)
{
#if defined(Q_OS_LINUX)
	QMap<QString, QString> m;
	m["apt"] = "apt-get install";
	m["zypp"] = "zypper install";
	m["yum"] = "yum install";
	m["urpmi"] = "urpmi";
	m["slackpkg"] = "slackpkg install";
	m["slapt-get"] = "slapt-get --install";
	m["netpkg"] = "netpkg";
	m["equo"] = "equo install";
	m["pacman"] = "pacman -S";
	m["conary"] = "conary update";
	m["pisi"] = "pisi install";
	m["emerge"] = "emerge";
	m["lin"] = "lin";
	m["cast"] = "cast";
	m["pkg_add"] = "pkg_add -r";
	return m[manager];
#endif
	Q_ASSERT_X(false, "installProgramForPackageManager", "Unknown package manager");
	return QString();
	// TODO other package managers
}

bool stringListToPackageList(PackageList *packages, const QStringList& packagesIn, QList<const Package *> &packagesOut, QStringList &alreadyInstalledPackagesOut, QString *notFoundPackage)
{
	PackageMatcher matcher(packages);
	packagesOut.clear();
	alreadyInstalledPackagesOut.clear();

	// captures expressions like this: <name>[/<version>][#<platform>]
	// examples: qtbase/5.0.0#win32-g++
	//           qtjsbackend
	//           qtpim#linux-g++-32
	//           qtquick1/5.1.0
	QRegularExpression exp("([a-z0-9\\-_\\+]*)(/[a-z0-9\\-\\+\\.]*)?(#[a-z0-9\\-\\+]*)?");

	QRegularExpressionMatch match;

	InstalledPackages* installed = ConfigurationHandler::instance()->installedPackages();

	foreach (const QString& arg, packagesIn) {
        match = exp.match(arg);
		if (!match.hasMatch()) {
			if (notFoundPackage != 0) {
				*notFoundPackage = arg;
			}
			return false;
		}
		const QString id = match.captured(1);
        const QString version = match.captured(2).remove(0, 1);
		const QString platform = match.captured(3).remove(0, 1);
		if (installed->isPackageInstalled(id, version, platform)) {
			alreadyInstalledPackagesOut.append(arg);
			continue;
		}
		packagesOut.append(matcher.matchPackages(id, version, platform));
	}
	return true;
}

QString installationRoot(const QString &version, const QString &platform)
{
    QDir dir(ConfigurationHandler::instance()->installRoot());
    return dir.absoluteFilePath(platform + "/" + version);
}
QDir removalScriptsDirectory()
{
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    dir.cd("removalScripts");
    return dir;
}

void ensureExists(const QString &directory)
{
	QDir dir(directory);
	if (!dir.exists()) {
		dir.mkpath(dir.absolutePath());
	}
}
void removeDirectoryRecursive(QDir directory)
{
    foreach (const QString& entry, directory.entryList(QDir::Files)) {
        directory.remove(entry);
    }
    foreach (const QString& entry, directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        removeDirectoryRecursive(QDir(directory.absoluteFilePath(entry)));
    }
    QDir().rmdir(directory.absolutePath());
}
void removeEmptyRecursive(const QDir &dir)
{
    foreach (const QString& child, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        removeEmptyRecursive(QDir(dir.absoluteFilePath(child)));
    }
    if (dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Files).isEmpty()) {
        QDir().rmdir(dir.absolutePath());
    }
}

void installArchiveEntry(const KArchiveEntry *entry, const QString &destination)
{
    QDir temp = QDir::temp();
    Util::ensureExists(temp.absoluteFilePath("soqute"));
    temp.cd("soqute");
    Util::ensureExists(temp.absoluteFilePath("tmp"));
    temp.cd("tmp");

    if (entry->isDirectory()) {
        const KArchiveDirectory* directory = static_cast<const KArchiveDirectory*>(entry);
        directory->copyTo(temp.absoluteFilePath(directory->name()));
        if (QDir().exists(destination)) {
            QDir().rmdir(destination);
        }
        temp.cd(directory->name());
        mergeDirectoryInto(temp, QDir(destination));
        temp.removeRecursively();
    } else {
        const KArchiveFile* file = static_cast<const KArchiveFile*>(entry);
        file->copyTo(temp.absoluteFilePath(file->name()));
        if (QDir().exists(destination)) {
            QDir().remove(destination);
        }
        QFile::copy(temp.absoluteFilePath(file->name()), destination);
        temp.remove(file->name());
    }
}
void mergeDirectoryInto(const QDir &source, const QDir &destination)
{
    // ensure the directory itself exists
    if (!destination.exists()) {
        QDir().mkdir(destination.absolutePath());
    }
    // and then move all the contents
    foreach (const QFileInfo& entry, source.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (entry.isFile()) {
            QFile::copy(entry.absoluteFilePath(), destination.absoluteFilePath(entry.fileName()));
        } else {
            mergeDirectoryInto(QDir(entry.absoluteFilePath()), destination.absoluteFilePath(entry.fileName()));
        }
    }
}

template<typename T>
QList<T> removeDuplicatesFromList(const QList<T> &in)
{
    QList<T> out;
    foreach (const T& t, in) {
        if (!out.contains(t)) {
            out.append(t);
        }
    }
    return out;
}

}
