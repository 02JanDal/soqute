#include "packagematcher.h"

#include <QDebug>

#include "package.h"
#include "util_core.h"

PackageMatcher::PackageMatcher(PackageList *packages) : m_packages(packages)
{
}

PackagePointerList PackageMatcher::matchPackages(const QString &name, const QString &version,
												 const Platform &host, const Platform &target) const
{
	Platform hst = host;
	if (hst.isEmpty()) {
		hst = Util::currentPlatform();
	}

	PackagePointerList result;
	// pending packages are those where we still have to check whether there isn't any higher
	// version
	QMap<QString, PackagePointer> pending;
	const PackagePointerList packages = m_packages->entities();
	for (PackagePointer pkg : packages) {
		if (pkg->id() == name && pkg->host().fuzzyCompare(hst) && pkg->target().fuzzyCompare(target)) {
			if (version.isEmpty()) {
				const QString identifier = QString("%1#%2#%3").arg(pkg->id(), pkg->host().toString(), pkg->target().toString());
				if (pending.contains(identifier)) {
					if (Util::isVersionHigherThan(pkg->version(),
												  pending[identifier]->version())) {
						pending[identifier] = pkg;
					}
				} else {
					pending[identifier] = pkg;
				}
			} else if (version == pkg->version()) {
				result.append(pkg);
			}
		}
	}
	result.append(pending.values());
	return result;
}

PackagePointer PackageMatcher::matchSinglePackage(const QString &name, const QString &version,
												  const Platform &host, const Platform &target) const
{
	PackagePointerList results = matchPackages(name, version, host, target);
	if (results.isEmpty()) {
		return 0;
	}
	return results.at(0);
}
