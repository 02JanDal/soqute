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

private:
	static inline bool fuzzyCompareString(const QString &str1, const QString &str2)
	{
		return str1 == str2 || str1.isEmpty() || str2.isEmpty() || str1 == "*" || str2 == "*";
	}
};
inline uint SOQUTE_CORESHARED_EXPORT qHash(const Platform &platform)
{
	return qHash(platform.os + platform.arch + platform.compiler);
}
Q_DECLARE_METATYPE(Platform)

class SOQUTE_CORESHARED_EXPORT Dependency : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString id READ id CONSTANT)
	Q_PROPERTY(QString version READ version CONSTANT)

public:
	explicit Dependency(QObject *parent = 0);

	QString id() const
	{
		return m_id;
	}
	QString version() const
	{
		return m_version;
	}

private:
	friend class PackageList;
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

	Q_PROPERTY(QString id READ id CONSTANT)
	Q_PROPERTY(QString description READ description CONSTANT)
	Q_PROPERTY(QString version READ version CONSTANT)
	Q_PROPERTY(Platform host READ host CONSTANT)
	Q_PROPERTY(Platform target READ target CONSTANT)
	Q_PROPERTY(QUrl url READ url CONSTANT)
	Q_PROPERTY(QList<Dependency *> dependencies READ dependencies CONSTANT)
	Q_PROPERTY(NativeDependencies nativeDependencies READ nativeDependencies CONSTANT)

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

private:
	friend class PackageList;

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
	explicit PackageList(QObject *parent = 0);

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
