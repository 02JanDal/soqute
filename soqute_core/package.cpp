#include "package.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QUrl>

#include "util_core.h"

Package::Package(PackageList *list, QObject *parent) :
    QObject(parent), m_list(list)
{
}

QList<const Package *> Package::recursiveDependencies() const
{
    QList<const Package*> deps;
	for (const Dependency* dep : m_dependencies) {
        const Package* pkg = m_list->package(dep, m_platform);
		for (PackagePointer pkgDep : pkg->recursiveDependencies()) {
            deps.prepend(pkgDep);
        }
        deps.prepend(pkg);
    }
    return Util::cleanPackagePointerList(deps);
}

PackageList::PackageList(QObject *parent) :
    QObject(parent)
{
}

Dependency::Dependency(QObject *parent) :
    QObject(parent)
{
}


bool PackageList::parse(const QByteArray &data)
{
    // create the document and handle errors
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        emit parseError(error.errorString(), error.offset);
        return false;
    }

    // it's easier to work with an variant list than json directly
    QVariantList entities = doc.array().toVariantList();

	for (const QVariant& entity : entities) {
        QMap<QString, QVariant> map = entity.toMap();
        Package* meta = new Package(this);
        meta->setId(map["id"].toString());
        meta->setDescription(map["description"].toString());
        meta->setVersion(map["version"].toString());
        meta->setPlatform(map["platform"].toString());
        meta->setUrl(QUrl::fromUserInput(map["url"].toString()));
        if (map.contains("dependencies")) {
            QList<Dependency*> dependencies;
            QVariantList deps = map["dependencies"].toList();
			for (const QVariant& dep : deps) {
                QMap<QString, QVariant> depMap = dep.toMap();
                Dependency* dependency = new Dependency(meta);
                dependency->setId(depMap["id"].toString());
                dependency->setVersion(depMap["version"].toString());
                dependencies.append(dependency);
            }
            meta->setDependencies(dependencies);
        }
        if (map.contains("nativeDependencies")) {
            QMap<QString, QStringList> nativeDependencies;
            QMap<QString, QVariant> deps = map["nativeDependencies"].toMap();
			for (const QString& dep : deps.keys()) {
                const QString pkgsystem = dep;
                const QStringList ids = deps[dep].toStringList();
                nativeDependencies.insert(pkgsystem, ids);
            }
            meta->setNativeDependencies(nativeDependencies);
        }
        m_entities.append(meta);
    }

    emit entitiesChanged(m_entities);
    return true;
}

PackagePointer PackageList::package(const QString &id, const QString &version, const QString &platform) const
{
    QString pltfrm = platform;
    if (pltfrm.isNull() || pltfrm.isEmpty()) {
        pltfrm = Util::currentPlatform();
	}

    const Package* result = 0;
	for (PackagePointer entity : m_entities) {
        // check if the id and the platform match. empty platform means any platform (most likely sources)
        if (entity->id() == id && (entity->platform() == pltfrm || entity->platform().isEmpty())) {
			if (version.isNull() || version.isEmpty() || (version.endsWith('+') && Util::isVersionHigherThan(entity->version(), QString(version).remove('+')))) {
				// check if this version is higher than the previous one
				if (result == 0 || entity->version() == result->version() || Util::isVersionHigherThan(entity->version(), result->version())) {
                    result = entity;
                }
			} else if (entity->version() == QString(version).remove('+')) {
                // if a version is supplied and it matches this entities version we have found the right package
                return entity;
            }
        }
    }

    return result;
}

PackagePointer PackageList::package(const Dependency *dependency, const QString& platform) const
{
    return package(dependency->id(), dependency->version(), platform);
}

PackagePointerList PackageList::otherPackages(PackagePointer package) const
{
	PackagePointerList packages;

	for (PackagePointer entity : m_entities) {
        if (entity->id() == package->id() && entity != package) {
            packages.append(entity);
        }
    }

    return packages;
}

PackagePointerList PackageList::findPackages(const QStringList &queries, const bool matchDescription) const
{
	PackagePointerList packages;

	for (PackagePointer entity : m_entities) {
        if (matchInQueries(entity, queries, matchDescription)) {
            packages.append(entity);
        }
    }

    return packages;
}

bool PackageList::matchInQueries(PackagePointer package, const QStringList &queries, const bool matchDescription) const
{
	for (const QString& query : queries) {
        // match the id
        QRegularExpression exp(QString("^.*%1.*$").arg(query), QRegularExpression::CaseInsensitiveOption);
        if (exp.match(package->id()).hasMatch()) {
            return true;
        } else if (isPlatformString(query) && package->platform() == query) {
            return true;
        } else if (isVersionString(query) && package->version() == query) {
            return true;
        } else if (matchDescription && exp.match(package->description()).hasMatch()) {
            return true;
        }
    }

    return false;
}

bool PackageList::isPlatformString(const QString &string) const
{
    QRegularExpression exp(
                "^(linux|win32|wince.*|solaris|aix|hpux|hpuxi|macx|blackberry|android|cygwin|darwin|freebsd|irix|lynxos|netbsd|openbsd|qnx|sco|tru64|unixware|hurd)"
                "-(g\\+\\+|cc|msvc\\d{4}|xlc|.*-qcc|icc|acc|cxx|clang|llvm|xcode|kcc|pgcc|arm|g\\+\\+\\d\\d|lsb-g\\+\\+)(-.*)*$", QRegularExpression::CaseInsensitiveOption);
    return exp.match(string).hasMatch();
}

bool PackageList::isVersionString(const QString &string) const
{
    QRegularExpression exp("^\\d(\\.\\d){1,4}((beta|alpha|rc)\\d){0,1}$", QRegularExpression::CaseInsensitiveOption);
    return exp.match(string).hasMatch();
}

QDebug &operator <<(QDebug &d, const PackagePointer pkg)
{
    d.nospace() << "Package(id=" << pkg->id() << " version=" << pkg->version() << " platform=" << pkg->platform() << ")";
    return d.space();
}
