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
#include "json.h"

Package::Package(PackageList *list, QObject *parent) : QObject(parent), m_list(list)
{
}

QList<PackagePointer> Package::recursiveDependencies() const
{
	QList<const Package *> deps;
	for (const Dependency *dep : m_dependencies) {
		const Package *pkg = m_list->package(dep, m_host, m_target);
		for (PackagePointer pkgDep : pkg->recursiveDependencies()) {
			deps.prepend(pkgDep);
		}
		deps.prepend(pkg);
	}
	return Util::cleanPackagePointerList(deps);
}

PackageList::PackageList(QObject *parent) : QObject(parent)
{
}

Dependency::Dependency(QObject *parent) : QObject(parent)
{
}

void PackageList::parse(const QByteArray &data)
{
	try {
		const QJsonArray root = Json::ensureArray(Json::ensureDocument(data));
		for (const QJsonValue &val : root) {
			const QJsonObject object = Json::ensureObject(val);
			Package *meta = new Package(this);
			meta->setId(Json::ensureIsType<QString>(object, "id"));
			meta->setDescription(Json::ensureIsType<QString>(object, "description", ""));
			meta->setVersion(Json::ensureIsType<QString>(object, "version"));
			if (object.contains("host")) {
				Platform host;
				const QJsonObject hostObj = Json::ensureObject(object, "host");
				host.os = Json::ensureIsType<QString>(hostObj, "os");
				host.arch = Json::ensureIsType<QString>(hostObj, "arch");
				host.compiler = Json::ensureIsType<QString>(hostObj, "compiler");
				meta->setHost(host);
			}
			if (object.contains("target")) {
				Platform target;
				const QJsonObject targetObj = Json::ensureObject(object, "target");
				target.os = Json::ensureIsType<QString>(targetObj, "os");
				target.arch = Json::ensureIsType<QString>(targetObj, "arch");
				target.compiler = Json::ensureIsType<QString>(targetObj, "compiler");
				meta->setTarget(target);
			}
			meta->setUrl(Json::ensureIsType<QUrl>(object, "url", QUrl()));
			if (object.contains("dependencies")) {
				QList<Dependency *> dependencies;
				const QJsonArray deps = Json::ensureArray(object, "dependencies");
				for (const QJsonValue &dep : deps) {
					const QJsonObject depObj = Json::ensureObject(dep);
					Dependency *dependency = new Dependency(meta);
					dependency->setId(Json::ensureIsType<QString>(depObj, "id"));
					dependency->setVersion(Json::ensureIsType<QString>(depObj, "version"));
					dependencies.append(dependency);
				}
				meta->setDependencies(dependencies);
			}
			if (object.contains("nativeDependencies")) {
				NativeDependencies nativeDependencies;
				const QJsonObject deps = Json::ensureObject(object, "nativeDependencies");
				for (const QString &dep : deps.keys()) {
					const NativeDependencies::PackageManager mngr = dep;
					const NativeDependencies::Dependencies ids = Json::ensureIsArrayOf<QString>(deps, dep);
					nativeDependencies.packageManagers.insert(mngr, ids);
				}
				meta->setNativeDependencies(nativeDependencies);
			}
			m_entities.append(meta);
		}

		emit entitiesChanged(m_entities);
	} catch (Exception &e) {
		emit parseError(e.message());
	}
}

PackagePointer PackageList::package(const QString &id, const QString &version,
									const Platform &host, const Platform &target) const
{
	Platform hst = host;
	if (hst.isEmpty()) {
		hst = Util::currentPlatform();
	}

	const Package *result = 0;
	for (PackagePointer entity : m_entities) {
		if (entity->id() == id && entity->host().fuzzyCompare(hst) && entity->target().fuzzyCompare(target)) {
			if (version.isEmpty() ||
				(version.endsWith('+') &&
				 Util::isVersionHigherThan(entity->version(), QString(version).remove('+')))) {
				// check if this version is higher than the previous one
				if (result == 0 || entity->version() == result->version() ||
					Util::isVersionHigherThan(entity->version(), result->version())) {
					result = entity;
				}
			} else if (entity->version() == QString(version).remove('+')) {
				// if a version is supplied and it matches this entities version we have found
				// the right package
				return entity;
			}
		}
	}

	return result;
}

PackagePointer PackageList::package(const Dependency *dependency, const Platform &host, const Platform &target) const
{
	return package(dependency->id(), dependency->version(), host, target);
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

PackagePointerList PackageList::findPackages(const QStringList &queries,
											 const bool matchDescription) const
{
	PackagePointerList packages;

	for (PackagePointer entity : m_entities) {
		if (matchInQueries(entity, queries, matchDescription)) {
			packages.append(entity);
		}
	}

	return packages;
}

bool PackageList::matchInQueries(PackagePointer package, const QStringList &queries,
								 const bool matchDescription) const
{
	for (const QString &query : queries) {
		// match the id
		QRegularExpression exp(QString("^.*%1.*$").arg(query),
							   QRegularExpression::CaseInsensitiveOption);
		if (exp.match(package->id()).hasMatch()) {
			return true;
		} else if (isPlatformString(query) && package->host().fuzzyCompare(Platform::fromString(query))) {
			return true;
		} else if (isPlatformString(query) && package->target().fuzzyCompare(Platform::fromString(query))) {
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
		"^(linux|win32|wince.*|solaris|aix|hpux|hpuxi|macx|blackberry|android|cygwin|darwin|"
		"freebsd|irix|lynxos|netbsd|openbsd|qnx|sco|tru64|unixware|hurd)"
		"-(g\\+\\+|cc|msvc\\d{4}|xlc|.*-qcc|icc|acc|cxx|clang|llvm|xcode|kcc|pgcc|arm|g\\+\\+"
		"\\d\\d|lsb-g\\+\\+)(-.*)*$",
		QRegularExpression::CaseInsensitiveOption);
	return exp.match(string).hasMatch();
}

bool PackageList::isVersionString(const QString &string) const
{
	QRegularExpression exp("^\\d(\\.\\d){1,4}((beta|alpha|rc)\\d){0,1}$",
						   QRegularExpression::CaseInsensitiveOption);
	return exp.match(string).hasMatch();
}

QDebug &operator<<(QDebug &d, const Platform pkg)
{
	d.nospace() << "Platform(os=" << pkg.os << " arch=" << pkg.arch << " compiler=" << pkg.compiler << ")";
	return d.space();
}
QDebug &operator<<(QDebug &d, const PackagePointer pkg)
{
	d.nospace() << "Package(id=" << pkg->id() << " version=" << pkg->version()
				<< " host=" << pkg->host() << " target=" << pkg->target() << ")";
	return d.space();
}

Platform Platform::fromString(const QString &string)
{
	QRegularExpression exp("(.*)\\-(.*)\\-(.*)");
	QRegularExpressionMatch match = exp.match(string);
	return Platform(match.captured(1), match.captured(2), match.captured(3));
}
