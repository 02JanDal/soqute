#include "installnativedependenciestask.h"

#include <QStringList>
#include <QProcess>
#include <iostream>

#include "package.h"
#include "configurationhandler.h"
#include "util_core.h"

InstallNativeDependenciesTask::InstallNativeDependenciesTask(PackageList* packages, QObject *parent) :
	Task(packages, parent)
{
}

void InstallNativeDependenciesTask::start()
{
	const QString pkgManager = ConfigurationHandler::instance()->packageManager();
	QStringList packages;
	foreach (PackagePointer package, m_usedPackages) {
		packages.append(package->nativeDependencies()[pkgManager]);
	}
	packages.removeDuplicates();

	if (packages.isEmpty()) {
		delayedFinish();
		return;
	}

	QProcess* process = new QProcess(this);
	connect(process, SIGNAL(finished(int)), this, SIGNAL(finished()));
	connect(process, SIGNAL(finished(int)), process, SLOT(deleteLater()));
	std::cout << "Installing native packages using " << qPrintable(pkgManager) << ": " << qPrintable(packages.join(", ")) << std::endl;
	process->start(Util::installerProgramForPackageManager(pkgManager), QStringList() << "install" << packages);
}
