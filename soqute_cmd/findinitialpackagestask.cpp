#include "findinitialpackagestask.h"

#include <QRegularExpression>
#include <QDebug>
#include <iostream>

#include "packagematcher.h"
#include "configurationhandler.h"
#include "installedpackages.h"

FindInitialPackagesTask::FindInitialPackagesTask(PackageList *packages, QObject *parent) :
	Task(packages, parent)
{
}

void FindInitialPackagesTask::start()
{
	PackageMatcher matcher(m_packages);
	m_usedPackages.clear();

	// captures expressions like this: <name>[/<version>][#<platform>]
	// examples: qtbase/5.0.0#win32-g++
	//           qtjsbackend
	//           qtpim#linux-g++-32
	//           qtquick1/5.1.0
	QRegularExpression exp("([a-z0-9\\-_\\+]*)(/[a-z0-9\\-\\+\\.]*)?(#[a-z0-9\\-\\+]*)?");

	QRegularExpressionMatch match;

	InstalledPackages* installed = ConfigurationHandler::instance()->installedPackages();

	QStringList alreadyInstalledPackages;

	foreach (const QString& arg, m_arguments) {
		match = exp.match(arg);
		if (!match.hasMatch()) {
			emit invalidPackageArgument(arg);
			break;
		}
		const QString id = match.captured(1);
		const QString version = match.captured(2);
		const QString platform = match.captured(3).remove(0, 1);
		if (installed->isPackageInstalled(id, version, platform)) {
			alreadyInstalledPackages.append(arg);
			continue;
		}
		m_usedPackages.append(matcher.matchPackages(id, version, platform));
	}

	if (!alreadyInstalledPackages.isEmpty()) {
		std::cout << "The following packages are already installed on your system:\n"
				  << "\t" << qPrintable(alreadyInstalledPackages.join(", ")) << std::endl;
	}

	delayedFinish();
}
