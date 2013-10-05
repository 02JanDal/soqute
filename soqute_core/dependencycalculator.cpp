#include "dependencycalculator.h"

#include "package.h"
#include <QtCore/QList>
#include <QDebug>

#include "configurationhandler.h"
#include "installedpackages.h"

DependencyCalculator::DependencyCalculator(PackageList *packageList, PackagePointerList packages, QObject *parent) :
    QObject(parent), m_startingPoint(packages), m_packages(packageList)
{
}

bool DependencyCalculator::calculateDependencies(const Direction direction, const Package **notFoundPackage)
{
    InstalledPackages* installed = ConfigurationHandler::instance()->installedPackages();

    if (direction == Forward) {
        PackagePointerList result;
        result.append(m_startingPoint);
        foreach (PackagePointer package, m_startingPoint) {
            bool ok = true;
            PackagePointerList res = calculateDependenciesForward(package, &ok, notFoundPackage);
            if (!ok) {
                return false;
            }
            result.append(res);
        }

        m_result.clear();
        foreach (const Package* package, result) {
            if (!m_result.contains(package)) {
                m_result.append(package);
            }
        }
    } else {
        PackagePointerList res;

        foreach (PackagePointer package, m_startingPoint) {
            res.append(calculateDependenciesReverse(package));
        }

        m_result.clear();
        foreach (const Package* package, res) {
            if (!m_result.contains(package) && installed->isPackageInstalled(package->id(), package->version(), package->platform())) {
                m_result.append(package);
            }
        }
    }

    emit done();
    return true;
}

void DependencyCalculator::setStartingPoint(const PackagePointerList packages)
{
    m_startingPoint = packages;
}

PackagePointerList DependencyCalculator::calculateDependenciesForward(const Package *package, bool* ok, const Package **notFoundPackage)
{
    PackagePointerList packages;
    foreach (const Dependency* dependency, package->dependencies()) {
        const Package* pkg = m_packages->package(dependency, package->platform());
        if (pkg == 0) {
            emit noSuchPackage(dependency->id(), dependency->version(), package->platform());
            if (ok) {
                *ok = false;
                if (notFoundPackage) {
                    *notFoundPackage = m_packages->package(dependency, package->platform());
                }
            }
            return packages;
        }
        packages.append(pkg);
        PackagePointerList deps = calculateDependenciesForward(pkg, ok);
        if (*ok == false) {
            return packages;
        }
        packages.append(deps);
    }
    *ok = true;
    return packages;
}

PackagePointerList DependencyCalculator::calculateDependenciesReverse(const Package* package, const QString& intendent)
{
    PackagePointerList out;
    out.append(package);
    foreach (PackagePointer pkg, m_packages->entities()) {
        if (pkg->platform() == package->platform() && !out.contains(pkg)) {
            foreach (const Dependency* dep, pkg->dependencies()) {
                if (dep->id() == package->id() && dep->version() == package->version()) {
                    out.append(calculateDependenciesReverse(pkg, intendent + "  "));
                }
            }
        }
    }

    return out;
}
