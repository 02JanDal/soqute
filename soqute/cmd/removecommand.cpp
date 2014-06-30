#include "removecommand.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>
#include <QDir>
#include <QCommandLineParser>

#include <iostream>

#include "package.h"
#include "remover.h"
#include "textstream.h"
#include "util_cmd.h"
#include "util_core.h"
#include "configurationhandler.h"
#include "installedpackages.h"
#include "dependencycalculator.h"

// FIXME dependency checking (removing qtgui = removing qtgui AND qtwidgets)

RemoveCommand::RemoveCommand(ConfigurationHandler *configHandler, PackageList *packages,
							 QObject *parent)
	: BaseCommand(configHandler, packages, parent)
{
}

void RemoveCommand::setupParser()
{
	parser = new QCommandLineParser();
	parser->setApplicationDescription(tr("Removes packages"));
	parser->addVersionOption();
	parser->addHelpOption();
	parser->addOption(QCommandLineOption(
		QStringList() << "no-platform-behavior",
		tr("Describes how I should behave if you don't give me a target platform. Possible options: "
		   "host (use the host platform, the default), abort"),
		tr("behavior"), "host"));
	parser->addOption(QCommandLineOption(
		QStringList() << "no-version-behavior",
		tr("Describes how I should behave if you don't give me a version. Possible options: "
		   "clean (removes all except the newest), all (removes all), abort (the default)"),
		tr("behavior"), "abort"));
	parser->addOption(
		QCommandLineOption(QStringList() << "silent", tr("Won't ask you for questions")));
	parser->addPositionalArgument(tr("package identifiers"),
								  tr("Identifiers for all packages to remove"),
								  tr("<package-name>[/<version>][#<target>]*"));
}

bool RemoveCommand::executeImplementation()
{
	if (parser->positionalArguments().isEmpty()) {
		out << "You need to specify at least one query" << endl;
		return false;
	}

	PackagePointerList pkgs;
	// argument parsing
	{
		// captures expressions like this: <name>[/<version>][#<target>]
		// examples: qtbase/5.0.0#win32-g++
		//           qtjsbackend
		//           qtpim#linux-g++-32
		//           qtquick1/5.1.0
		QRegularExpression exp("([a-z0-9\\-_\\+]*)(/[a-z0-9\\-\\+\\.]*)?(#[a-z0-9\\-\\+]*)?");

		for (PackagePointer entity : packages->entities()) {
			for (const QString &argument : parser->positionalArguments()) {
				QRegularExpressionMatch match = exp.match(argument);
				const QString id = match.captured(1);
				const QString version = match.captured(2).remove(0, 1);
				const QString target = match.captured(3).remove(0, 1);

				if (parser->value("no-platform-behavior") == "abort" && target.isEmpty()) {
					out << "You need to specify a platform for the following argument: "
						<< argument << endl;
					return false;
				}

				if (parser->value("no-version-behavior") == "abort" && version.isEmpty()) {
					out << "You need to specify a version for the following argument: "
						<< argument << ", or check out --no-version-behavior" << endl;
					return false;
				}

				if (entity->id() == id) {
					if ((parser->value("no-platform-behavior") == "host" &&
						 entity->target().fuzzyCompare(Util::currentPlatform()))) {
						// clean, all
						if (parser->value("no-version-behavior") == "all" ||
							(parser->value("no-version-behavior") == "clean" &&
							 isNewestInstalled(entity)) ||
							entity->version() == version) {
							if (ConfigurationHandler::instance()
									->installedPackages()
									->isPackageInstalled(entity)) {
								pkgs.append(entity);
							}
						}
					}
				}
			}
		}
	}

	pkgs = Util::removeDuplicatesFromList(pkgs);

	// dependency calculation
	{
		DependencyCalculator depCalculator(packages, pkgs, this);
		if (!depCalculator.calculateDependencies(DependencyCalculator::Reverse)) {
			return false;
		}
		pkgs = depCalculator.result();
	}

	pkgs = Util::removeDuplicatesFromList(pkgs);

	if (pkgs.isEmpty()) {
		out << "Nothing to do" << endl;
		return true;
	}

	// confirmation
	{
		if (!parser->isSet("silent")) {
			out << "You have chosen to remove the following packages:\n  " << endl;
			QStringList packages;
			for (PackagePointer package : pkgs) {
				packages.append(Util::createFriendlyName(package));
			}
			out << packages.join(", ") << endl;

			if (!Util::confirm()) {
				return false;
			}
		}
	}

	// remove
	{
		Remover remover(this);
		remover.remove(pkgs);

		while (!remover.isDone()) {
			qApp->processEvents();
			qApp->sendPostedEvents();
			while (remover.hasMessage()) {
				out << remover.messageToString(remover.takeLastMessage()) << endl << flush;
			}
		}
		while (remover.hasMessage()) {
			out << remover.messageToString(remover.takeLastMessage()) << endl;
		}
	}

	return true;
}

bool RemoveCommand::isNewestInstalled(PackagePointer entity)
{
	return ConfigurationHandler::instance()->installedPackages()->isNewestInstalled(entity);
}
