#ifndef PACKAGEMATCHER_H
#define PACKAGEMATCHER_H

#include <QtCore/QList>

#include "soqute_core_global.h"

class PackageList;
class Package;

class SOQUTE_CORESHARED_EXPORT PackageMatcher
{
public:
    PackageMatcher(PackageList *packages);

    QList<const Package*> matchPackages(const QString& name, const QString& version, const QString& platform) const;
    const Package* matchSinglePackage(const QString& name, const QString& version, const QString& platform) const;

private:
    PackageList* m_packages;
};

#endif // PACKAGEMATCHER_H
