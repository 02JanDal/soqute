#include "installcommandbase.h"

#include <QProcess>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QCoreApplication>

#include <iostream>

#include "downloader.h"
#include "util_core.h"
#include "util_cmd.h"
#include "dependencycalculator.h"
#include "package.h"
#include "configurationhandler.h"
#include "installedpackages.h"
#include "filesystem.h"
#include "textstream.h"

InstallCommandBase::InstallCommandBase(ConfigurationHandler *configHandler,
									   PackageList *packages, QObject *parent)
	: BaseCommand(configHandler, packages, parent)
{
}

void InstallCommandBase::baseChecks()
{
	// make sure the base install directory is available
	FS::ensureExists(ConfigurationHandler::instance()->installRoot());

	if (ConfigurationHandler::instance()->repositoryUrls().isEmpty()) {
		throw Exception(tr("No repositories available"));
	}
}

void InstallCommandBase::constructPackageList(const QStringList cliPackages)
{
	// handle arguments
	{
		QString notFoundPackage;
		QStringList alreadyInstalledPackagesOut;
		if (!Util::stringListToPackageList(packages, cliPackages, m_pkgs,
										   alreadyInstalledPackagesOut, &notFoundPackage)) {
			throw Exception(
				tr("The following package could not be found: %1").arg(notFoundPackage));
		}
	}

	// dependency calculation
	{
		PackagePointerList tmp = m_pkgs;
		for (PackagePointer pkg : m_pkgs) {
			tmp.append(pkg->recursiveDependencies());
		}
		m_pkgs = Util::cleanPackagePointerList(tmp);
	}

	// remove installed packages
	{
		QMutableListIterator<PackagePointer> it(m_pkgs);
		while (it.hasNext()) {
			PackagePointer pkg = it.next();
			if (ConfigurationHandler::instance()->installedPackages()->isPackageInstalled(
					pkg->id(), pkg->version(), pkg->platform())) {
				it.remove();
			}
		}
	}
}

void InstallCommandBase::installPackages(AbstractAuthenticator *authenticator)
{
	// install native dependencies
	{
		const QString pkgManager = ConfigurationHandler::instance()->packageManager();
		QStringList packages;
		for (PackagePointer package : m_pkgs) {
			packages.append(package->nativeDependencies()[pkgManager]);
		}
		packages.removeDuplicates();

		if (!packages.isEmpty()) {
			QProcess *process = new QProcess(this);
			out << "Installing native packages using " << pkgManager << ": "
				<< packages.join(", ") << endl;
			process->start(Util::installerProgramForPackageManager(pkgManager),
						   QStringList() << packages);
		}
	}

	// install
	{
		Downloader downloader(packages, m_pkgs, this);
		downloader.setAuthenticator(authenticator);
		downloader.downloadAndInstall();

		while (!downloader.isDone()) {
			qApp->processEvents();
			qApp->sendPostedEvents();
			while (downloader.hasMessage()) {
				out << downloader.messageToString(downloader.takeLastMessage()) << endl;
			}
		}
		while (downloader.hasMessage()) {
			out << downloader.messageToString(downloader.takeLastMessage()) << endl;
		}

		if (!downloader.isSuccess()) {
			throw Exception("Error installing packages");
		}
	}
}

bool CommandLineAuthenticator::authenticate(QNetworkReply *reply, QAuthenticator *authenticator)
{
	out << "You need to authenticate to access " << reply->url().toString() << ".\n";
	if (!authenticator->realm().isNull()) {
		out << "The server says: \"" << authenticator->realm() << "\""
			<< "\n";
	}
	out << "Username: " << flush;
	const unsigned char size = 64;
	char buf[size];
	std::cin.getline(buf, size);
	authenticator->setUser(QString::fromLocal8Bit(buf, size));
	std::cin.getline(buf, size);
	authenticator->setPassword(QString::fromLocal8Bit(buf, size));
	return true;
}
bool FailAuthenticator::authenticate(QNetworkReply *, QAuthenticator *)
{
	out << QObject::tr("Network authentication is not allowed in this mode") << endl;
	return false;
}
