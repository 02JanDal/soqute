#pragma once

#include <QList>

#include "soqute_core_global.h"

class PackageList;
class Package;
class Platform;

class SOQUTE_CORESHARED_EXPORT PackageMatcher
{
public:
	PackageMatcher(PackageList *packages);

	QList<const Package *> matchPackages(const QString &name, const QString &version,
										 const Platform &host, const Platform &target) const;
	const Package *matchSinglePackage(const QString &name, const QString &version,
									  const Platform &host, const Platform &target) const;

private:
	PackageList *m_packages;
};
