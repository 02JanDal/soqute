#include "installcommand.h"

#include <QProcess>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QDebug>

#include <cstdio>
#include <iostream>

#include "qcommandlineparser.h"
#include "downloader.h"
#include "util_core.h"
#include "util_cmd.h"
#include "dependencycalculator.h"
#include "package.h"
#include "configurationhandler.h"
#include "installedpackages.h"
#include "textstream.h"

class CommandLineAuthenticator : public AbstractAuthenticator
{
public:
	void authenticate(QNetworkReply *reply, QAuthenticator *authenticator)
	{
		out << "You need to authenticate to access " << reply->url().toString() << ".\n";
		if (!authenticator->realm().isNull()) {
			out << "The server says: \"" << authenticator->realm() << "\"" << "\n";
		}
		out << "Username: " << flush;
		const unsigned char size = 64;
		char buf[size];
		std::cin.getline(buf, size);
		authenticator->setUser(QString::fromLocal8Bit(buf, size));
		std::cin.getline(buf, size);
		authenticator->setPassword(QString::fromLocal8Bit(buf, size));
	}
};

InstallCommand::InstallCommand(ConfigurationHandler *configHandler, PackageList* packages, QObject *parent) :
	BaseCommand(configHandler, packages, parent)
{
}

void InstallCommand::setupParser()
{
    parser = new QCommandLineParser();
    parser->addVersionOption();
    parser->addHelpOption(tr("Installs one or more packages"));
    parser->setRemainingArgumentsHelpText(tr("<package>*\n\n\t<package> = <package-name>[/<version>][#<platform>]"));
	parser->addOption(QCommandLineOption(QStringList() << "s" << "silent", tr("Does not ask for any information, no user interaction required")));
}

bool InstallCommand::executeImplementation()
{
    // make sure the base install directory is available
	Util::ensureExists(ConfigurationHandler::instance()->installRoot());

	PackagePointerList pkgs;

	// argument parsing
	{
		QString notFoundPackage;
        QStringList alreadyInstalledPackagesOut;
		if (!Util::stringListToPackageList(packages, parser->remainingArguments(), pkgs, alreadyInstalledPackagesOut, &notFoundPackage)) {
			out << "The following package could not be found: " << notFoundPackage << "\n" << flush;
			return false;
		}
	}

	// dependency calculation
	{
        PackagePointerList tmp = pkgs;
        foreach (PackagePointer pkg, pkgs) {
            tmp.append(pkg->recursiveDependencies());
        }
        pkgs = Util::cleanPackagePointerList(tmp);
	}

    // remove installed packages
    {
        foreach (PackagePointer pkg, pkgs) {
            if (ConfigurationHandler::instance()->installedPackages()->isPackageInstalled(pkg->id(), pkg->version(), pkg->platform())) {
                pkgs.removeAll(pkg);
            }
        }
    }

    if (pkgs.isEmpty()) {
        out << "Nothing to do\n" << flush;
        return true;
    }

	// confirmation
	{
		if (!parser->isSet("silent")) {
			out << "You have choosen to install the following packages:\n"
				<< "  " << flush;
			QStringList packages;
			foreach (PackagePointer package, pkgs) {
				packages.append(QString("%1 v%2 (%3)").arg(package->id(), package->version(), package->platform()));
			}
            out << packages.join(", ") << '\n' << flush;

            if (!Util::confirm()) {
                return false;
            }
		}
	}

	// install native dependencies
	{
		const QString pkgManager = ConfigurationHandler::instance()->packageManager();
		QStringList packages;
		foreach (PackagePointer package, pkgs) {
			packages.append(package->nativeDependencies()[pkgManager]);
		}
		packages.removeDuplicates();

		if (!packages.isEmpty()) {
			QProcess* process = new QProcess(this);
			out << "Installing native packages using " << pkgManager << ": " << packages.join(", ") << "\n" << flush;
			process->start(Util::installerProgramForPackageManager(pkgManager), QStringList() << packages);
		}
	}

	// install
	{
        Downloader downloader(packages, pkgs, this);
		downloader.setAuthenticator(new CommandLineAuthenticator);
		downloader.downloadAndInstall();

		while (!downloader.isDone()) {
			qApp->processEvents();
			qApp->sendPostedEvents();
			while (downloader.hasMessage()) {
				QPair<Downloader::Message, QVariant> msg = downloader.takeLastMessage();
				out << downloader.messageToString(msg.first, msg.second) << "\n" << flush;
			}
		}
		while (downloader.hasMessage()) {
			QPair<Downloader::Message, QVariant> msg = downloader.takeLastMessage();
			out << downloader.messageToString(msg.first, msg.second) << "\n" << flush;
		}
	}

	return true;
}
