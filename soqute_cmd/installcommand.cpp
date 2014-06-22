#include "installcommand.h"

#include <QDebug>
#include <QCommandLineParser>

#include "textstream.h"
#include "exception.h"
#include "util_cmd.h"

InstallCommand::InstallCommand(ConfigurationHandler *configHandler, PackageList *packages,
							   QObject *parent)
	: InstallCommandBase(configHandler, packages, parent)
{
}

void InstallCommand::setupParser()
{
	parser = new QCommandLineParser();
	parser->setApplicationDescription(tr("Installs one or more packages"));
	parser->addVersionOption();
	parser->addHelpOption();
	parser->addPositionalArgument(tr("package identifiers"),
								  tr("Identifiers for all packages to install"),
								  tr("<package-name>[/<version>][#<platform>]*"));
	parser->addOption(QCommandLineOption(
		QStringList() << "s"
					  << "silent",
		tr("Does not ask for any information, no user interaction required")));
}

bool InstallCommand::executeImplementation()
{
	try
	{
		baseChecks();

		constructPackageList(parser->positionalArguments());

		if (m_pkgs.isEmpty()) {
			out << "Nothing to do" << endl;
			return true;
		}

		// confirmation
		{
			if (!parser->isSet("silent")) {
				out << "You have choosen to install the following packages:\n  ";
				QStringList packages;
				for (PackagePointer package : m_pkgs) {
					packages.append(QString("%1 v%2 (%3)").arg(
						package->id(), package->version(), package->platform()));
				}
				out << packages.join(", ") << endl;

				if (!Util::confirm()) {
					return false;
				}
			}
		}

		installPackages(new CommandLineAuthenticator);
	}
	catch (Exception &e)
	{
		out << "Error: " << e.message() << endl;
		return false;
	}
	return true;
}
