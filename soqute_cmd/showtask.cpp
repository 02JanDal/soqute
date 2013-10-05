#include "showtask.h"

#include <QtCore/QTextStream>
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <iostream>

#include "package.h"
#include "util.h"
#include "configurationhandler.h"
#include "installedpackages.h"
#include "dependencycalculator.h"

ShowTask::ShowTask(PackageList *packages, QObject *parent) :
    Task(packages, parent)
{
}

void ShowTask::setOutputNativePackagesOnly(const QString &manager)
{
    m_pkgManager = manager;
}
void ShowTask::setPackageId(const QString &id)
{
    m_packageId = id;
}
void ShowTask::setPackageVersion(const QString &version)
{
	m_packageVersion = version;
}
void ShowTask::setPackagePlatform(const QString &platform)
{
    m_packagePlatform = platform;
}

void ShowTask::start()
{
    QTextStream str(stdout);
	const Package* package = m_packages->package(m_packageId, m_packageVersion, m_packagePlatform);

    if (package == 0) {
        str << "No such package found\n";
        delayedFinish();
        return;
    }

	PackagePointerList packages;
	foreach (const Dependency* dep, package->dependencies()) {
		packages.append(m_packages->package(dep, package->platform()));
	}

	DependencyCalculator depCalculator(m_packages, packages, this);
	depCalculator.calculateDependencies();

    if (m_pkgManager.isNull()) {
        // populate the other versions/platforms lists
        const QList<const Package*> others = m_packages->otherPackages(package);
        QStringList allPlatforms;
        QStringList allVersions;
        foreach (const Package* pkg, others) {
            allPlatforms.append(pkg->platform());
            allVersions.append(pkg->version());
        }
        allPlatforms.removeDuplicates();
        allPlatforms.removeAll(package->platform());
        allVersions.removeDuplicates();
        allVersions.removeAll(package->version());

        // just output everything
		str << "Status:              "
			<< (ConfigurationHandler::instance()->installedPackages()->isPackageInstalled(package->id(), package->version(), package->platform())
			   ? "Installed" : "Not installed")
				 << "\n";
        str << "ID:                  " << package->id() << "\n"
            << "Description:         " << package->description() << "\n"
            << "Version:             " << package->version() << "\n"
            << "Other versions:      " << allVersions.join(", ") << "\n"
            << "Platform:            " << package->platform() << "\n"
            << "Other platforms:     " << allPlatforms.join(", ") << "\n"
			<< "Dependencies:        ";
		foreach (const PackagePointer p, depCalculator.result()) {
			str << p->id() << " ";
        }
        str << "\n"
            << "Native dependencies: \n";
		QMap<QString, QStringList> nDeps;
		foreach (const PackagePointer p, depCalculator.result()) {
			foreach (const QString& mngr, p->nativeDependencies().keys()) {
				nDeps[mngr].append(p->nativeDependencies()[mngr]);
			}
		}

		foreach (const QString& mngr, nDeps.keys()) {
			str << "    " << mngr << ": " << nDeps[mngr].join(", ") << "\n";
        }
    } else {
        // only output packages
		QMap<QString, QStringList> nDeps;
		foreach (const PackagePointer p, depCalculator.result()) {
			foreach (const QString& mngr, p->nativeDependencies().keys()) {
				nDeps[mngr].append(p->nativeDependencies()[mngr]);
			}
		}
		QStringList packages = nDeps[m_pkgManager];
        str << packages.join(' ') << "\n";
    }

    // we need to delay this until this function has finished
    delayedFinish();
}
