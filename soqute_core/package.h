#pragma once

#include <QObject>
#include <QMap>
#include <QStringList>
#include <QUrl>

#include "soqute_core_global.h"

class Dependency;
class Package;
class PackageList;
typedef const Package *PackagePointer;
typedef QList<PackagePointer> PackagePointerList;

struct SOQUTE_CORESHARED_EXPORT Platform
{
	Platform(const QString &os, const QString &arch, const QString &compiler) : os(os), arch(arch), compiler(compiler) {}
	Platform() {}
	QString os;
	QString arch;
	QString compiler;
	bool operator==(const Platform &other) const
	{
		return os == other.os && arch == other.arch && compiler == other.compiler;
	}
	bool operator!=(const Platform &other) const
	{
		return !operator==(other);
	}
	bool operator<(const Platform &other) const
	{
		return (os + arch + compiler) < (other.os + other.arch + other.compiler);
	}
	bool fuzzyCompare(const Platform &other) const
	{
		auto fuzzyCompareString = [](const QString &str1, const QString &str2)
		{
			return str1 == str2 || str1.isEmpty() || str2.isEmpty() || str1 == "*" || str2 == "*";
		};
		return fuzzyCompareString(os, other.os) && fuzzyCompareString(arch, other.arch) && fuzzyCompareString(compiler, other.compiler);
	}
	bool isEmpty() const
	{
		return os.isEmpty() && arch.isEmpty() && compiler.isEmpty();
	}
	QString toString() const
	{
		return os + '-' + arch + '-' + compiler;
	}
	static Platform fromString(const QString &string);
};
inline uint SOQUTE_CORESHARED_EXPORT qHash(const Platform &platform)
{
	return qHash(platform.os + platform.arch + platform.compiler);
}
Q_DECLARE_METATYPE(Platform)

class SOQUTE_CORESHARED_EXPORT Dependency : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
	Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)

public:
	Dependency(QObject *parent = 0);

	QString id() const
	{
		return m_id;
	}
	QString version() const
	{
		return m_version;
	}

signals:
	void idChanged(QString arg);
	void versionChanged(QString arg);

public
slots:
	void setId(QString arg)
	{
		if (m_id != arg) {
			m_id = arg;
			emit idChanged(arg);
		}
	}
	void setVersion(QString arg)
	{
		if (m_version != arg) {
			m_version = arg;
			emit versionChanged(arg);
		}
	}

private:
	QString m_id;
	QString m_version;
};

struct SOQUTE_CORESHARED_EXPORT NativeDependencies
{
	typedef QString PackageManager;
	typedef QStringList Dependencies;
	QMap<PackageManager, Dependencies> packageManagers;

	QList<PackageManager> keys() const { return packageManagers.keys(); }
	Dependencies operator[](const PackageManager &mngr) const { return packageManagers[mngr]; }
	bool operator!=(const NativeDependencies &other) const
	{
		return packageManagers != other.packageManagers;
	}
};

// QUESTION rename to something else? like Entity?
class SOQUTE_CORESHARED_EXPORT Package : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
	Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY
				   descriptionChanged)
	Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)
	Q_PROPERTY(Platform host READ host WRITE setHost NOTIFY hostChanged)
	Q_PROPERTY(Platform target READ target WRITE setTarget NOTIFY targetChanged)
	Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
	Q_PROPERTY(QList<Dependency *> dependencies READ dependencies WRITE setDependencies NOTIFY
				   dependenciesChanged)
	Q_PROPERTY(NativeDependencies nativeDependencies READ nativeDependencies WRITE
				   setNativeDependencies NOTIFY nativeDependenciesChanged)

public:
	explicit Package(PackageList *list, QObject *parent = 0);

	QString id() const
	{
		return m_id;
	}
	QString description() const
	{
		return m_description;
	}
	QString version() const
	{
		return m_version;
	}
	Platform host() const
	{
		return m_host;
	}
	Platform target() const
	{
		return m_target;
	}
	QUrl url() const
	{
		return m_url;
	}
	QList<Dependency *> dependencies() const
	{
		return m_dependencies;
	}
	NativeDependencies nativeDependencies() const
	{
		return m_nativeDependencies;
	}

	QList<PackagePointer> recursiveDependencies() const;

signals:
	void idChanged(QString arg);
	void descriptionChanged(QString arg);
	void versionChanged(QString arg);
	void hostChanged(Platform arg);
	void targetChanged(Platform arg);
	void urlChanged(QUrl arg);
	void dependenciesChanged(QList<Dependency *> arg);
	void nativeDependenciesChanged(NativeDependencies arg);

public
slots:
	void setId(QString arg)
	{
		if (m_id != arg) {
			m_id = arg;
			emit idChanged(arg);
		}
	}
	void setDescription(QString arg)
	{
		if (m_description != arg) {
			m_description = arg;
			emit descriptionChanged(arg);
		}
	}
	void setVersion(QString arg)
	{
		if (m_version != arg) {
			m_version = arg;
			emit versionChanged(arg);
		}
	}
	void setHost(Platform arg)
	{
		if (m_host != arg) {
			m_host = arg;
			emit hostChanged(arg);
		}
	}
	void setTarget(Platform arg)
	{
		if (m_target != arg) {
			m_target = arg;
			emit targetChanged(arg);
		}
	}

	void setUrl(QUrl arg)
	{
		if (m_url != arg) {
			m_url = arg;
			emit urlChanged(arg);
		}
	}
	void setDependencies(QList<Dependency *> arg)
	{
		if (m_dependencies != arg) {
			m_dependencies = arg;
			emit dependenciesChanged(arg);
		}
	}
	void setNativeDependencies(NativeDependencies arg)
	{
		if (m_nativeDependencies != arg) {
			m_nativeDependencies = arg;
			emit nativeDependenciesChanged(arg);
		}
	}

private:
	PackageList *m_list;
	QString m_id;
	QString m_description;
	QString m_version;
	Platform m_host;
	Platform m_target;
	QUrl m_url;
	QList<Dependency *> m_dependencies;
	NativeDependencies m_nativeDependencies;
};

Q_DECLARE_METATYPE(PackagePointer)

class SOQUTE_CORESHARED_EXPORT PackageList : public QObject
{
	Q_OBJECT

	Q_PROPERTY(PackagePointerList entities READ entities NOTIFY entitiesChanged)

public:
	PackageList(QObject *parent = 0);

	void parse(const QByteArray &data);

	PackagePointerList entities() const
	{
		return m_entities;
	}

	/**
	 * \returns The package with the highest version (if no version is given) that matches the
	 * platform and id supplied
	 */
	PackagePointer package(const QString &id, const QString &version = QString(),
						   const Platform &host = Platform(), const Platform &target = Platform()) const;

	/**
	 * \returns The package associated with the dependency, or null if no such package exists
	 */
	PackagePointer package(const Dependency *dependency, const Platform &host, const Platform &target) const;

	/**
	 * \returns Other packages with the same id (but other versions/platforms)
	 */
	PackagePointerList otherPackages(PackagePointer package) const;

	/**
	 * \param queries All queries that could be matched
	 * \param matchDescription True if the description should be searched
	 * \returns A list of all matching packages
	 */
	PackagePointerList findPackages(const QStringList &queries,
									const bool matchDescription) const;

	// Internal functions used by RemoveCommand
	// TODO Move to Util?
	bool matchInQueries(PackagePointer, const QStringList &queries,
						const bool matchDescription) const;
	bool isPlatformString(const QString &string) const;
	bool isVersionString(const QString &string) const;

signals:
	void entitiesChanged(PackagePointerList arg);
	void parseError(const QString &error);

public
slots:

private:
	PackagePointerList m_entities;
};

QDebug &operator<<(QDebug &d, const PackagePointer pkg);
