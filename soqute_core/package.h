#ifndef PACKAGE_H
#define PACKAGE_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QStringList>
#include <QUrl>

#include "soqute_core_global.h"

class Dependency;
class Package;
class PackageList;

class SOQUTE_CORESHARED_EXPORT Dependency : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)

public:
    Dependency(QObject* parent = 0);

    QString id() const { return m_id; }
    QString version() const { return m_version; }

signals:
    void idChanged(QString arg);
    void versionChanged(QString arg);

public slots:
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

// needed to put it into Q_PROPERTY
// TODO make a QObject?
typedef QMap<QString, QStringList> StringStringListMap;

// QUESTION rename to something else? like Entity?
class SOQUTE_CORESHARED_EXPORT Package : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)
    // TODO might support multiple platforms. different packages or expend this property?
    Q_PROPERTY(QString platform READ platform WRITE setPlatform NOTIFY platformChanged)
	Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QList<Dependency*> dependencies READ dependencies WRITE setDependencies NOTIFY dependenciesChanged)
    Q_PROPERTY(StringStringListMap nativeDependencies READ nativeDependencies WRITE setNativeDependencies NOTIFY nativeDependenciesChanged)

public:
    explicit Package(PackageList* list, QObject *parent = 0);
    
    QString id() const { return m_id; }
    QString description() const { return m_description; }
    QString version() const { return m_version; }
    QString platform() const { return m_platform; }
	QUrl url() const { return m_url; }
    QList<Dependency*> dependencies() const { return m_dependencies; }
    QMap<QString, QStringList> nativeDependencies() const { return m_nativeDependencies; }

    QList<const Package*> recursiveDependencies() const;

signals:
    void idChanged(QString arg);
    void descriptionChanged(QString arg);
    void versionChanged(QString arg);
    void platformChanged(QString arg);
	void urlChanged(QUrl arg);
    void dependenciesChanged(QList<Dependency*> arg);
	void nativeDependenciesChanged(QMap<QString, QStringList> arg);

public slots:
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
    void setPlatform(QString arg)
    {
        if (m_platform != arg) {
            m_platform = arg;
            emit platformChanged(arg);
        }
    }
	void setUrl(QUrl arg)
	{
		if (m_url != arg) {
			m_url = arg;
			emit urlChanged(arg);
		}
	}
    void setDependencies(QList<Dependency*> arg)
    {
        if (m_dependencies != arg) {
            m_dependencies = arg;
            emit dependenciesChanged(arg);
        }
    }
    void setNativeDependencies(QMap<QString, QStringList> arg)
    {
        if (m_nativeDependencies != arg) {
            m_nativeDependencies = arg;
            emit nativeDependenciesChanged(arg);
        }
	}

private:
    PackageList* m_list;
    QString m_id;
    QString m_description;
    QString m_version;
    QString m_platform;
	QUrl m_url;
    QList<Dependency*> m_dependencies;
	QMap<QString, QStringList> m_nativeDependencies;
};

typedef const Package* PackagePointer;
typedef QList<PackagePointer> PackagePointerList;

Q_DECLARE_METATYPE(PackagePointer)

class SOQUTE_CORESHARED_EXPORT PackageList : public QObject
{
    Q_OBJECT

	Q_PROPERTY(PackagePointerList entities READ entities NOTIFY entitiesChanged)

public:
    PackageList(QObject* parent = 0);

    bool parse(const QByteArray& data);

	PackagePointerList entities() const { return m_entities; }

    /**
     * \returns The package with the highest version (if no version is given) that matches the platform and id supplied
     */
	PackagePointer package(const QString& id, const QString& version = QString(), const QString& platform = QString()) const;

    /**
     * \returns The package associated with the dependency, or null if no such package exists
     */
	 PackagePointer package(const Dependency* dependency, const QString &platform) const;

    /**
     * \returns Other packages with the same id (but other versions/platforms)
     */
	PackagePointerList otherPackages(PackagePointer package) const;

    /**
     * \param queries All queries that could be matched
     * \param matchDescription True if the description should be searched
     * \returns A list of all matching packages
     */
	PackagePointerList findPackages(const QStringList& queries, const bool matchDescription) const;

    // Internal functions used by RemoveCommand
    // TODO Move to Util?
    bool matchInQueries(PackagePointer, const QStringList& queries, const bool matchDescription) const;
    bool isPlatformString(const QString& string) const;
    bool isVersionString(const QString& string) const;

signals:
	void entitiesChanged(PackagePointerList arg);
    void parseError(const QString& error, const int offset);

public slots:

private:
    PackagePointerList m_entities;
};

QDebug& operator<<(QDebug& d, const PackagePointer pkg);

#endif // PACKAGE_H
