#include "showcommand.h"

#include <QCoreApplication>
#include <QString>
#include <QRegularExpression>
#include <QDebug>
#include <QCommandLineParser>

#include "util_cmd.h"
#include "util_core.h"
#include "configurationhandler.h"
#include "package.h"
#include "dependencycalculator.h"
#include "installedpackages.h"
#include "textstream.h"

ShowCommand::ShowCommand(ConfigurationHandler *configHandler, PackageList *packages,
						 QObject *parent)
	: BaseCommand(configHandler, packages, parent)
{
}

void ShowCommand::setupParser()
{
	parser = new QCommandLineParser();
	parser->setApplicationDescription(tr("Shows information about a package"));
	parser->addVersionOption();
	parser->addHelpOption();
	parser->addPositionalArgument(tr("package identifer"), tr("The identifier of the package"),
								  tr("<package-name>[/<version>][#<target>]"));
	parser->addOption(QCommandLineOption(
		QStringList() << "nativeDependencies",
		tr("Only outputs the native packages for piping to the package manager"),
		tr("package-manager"), configHandler->packageManager()));
	parser->addOption(QCommandLineOption("host", tr("Show packages for the <platform> instead of the current platform"), tr("platform"), Util::currentPlatform()));
}

bool ShowCommand::executeImplementation()
{
	if (parser->positionalArguments().isEmpty()) {
		out << "You need to specify a package" << endl;
		return false;
	}

	QList<PackagePointer> pkgs;
	if (!Util::stringListToPackageList(packages, QStringList() << parser->positionalArguments()[0], pkgs, parser->value("host"), 0)) {
		out << "Invalid package identifier given" << endl;
		return false;
	}

	if (pkgs.isEmpty()) {
		out << "No such package found" << endl;
		return false;
	}

	PackagePointer package = pkgs.first();

	if (!parser->isSet("nativeDependencies")) {
		// populate the other versions/platforms lists
		const QList<const Package *> others = packages->otherPackages(package);
		QStringList allTargets;
		QStringList allVersions;
		for (PackagePointer pkg : others) {
			allTargets.append(pkg->target());
			allVersions.append(pkg->version());
		}
		allTargets.removeDuplicates();
		allTargets.removeAll(package->target());
		allVersions.removeDuplicates();
		allVersions.removeAll(package->version());

		// just output everything
		out << "Status:              "
			<< (ConfigurationHandler::instance()->installedPackages()->isPackageInstalled(
					package)
					? "Installed"
					: "Not installed") << endl;
		out << "ID:                  " << package->id() << endl
			<< "Description:         " << package->description() << endl
			<< "Version:             " << package->version() << endl
			<< "Other versions:      " << allVersions.join(", ") << endl
			<< "Target:              " << package->target() << endl
			<< "Other targets:       " << allTargets.join(", ") << endl
			<< "Dependencies:        ";
		for (PackagePointer p : package->recursiveDependencies()) {
			out << p->id() << " ";
		}
		out << endl << "Native dependencies: " << endl;
		QMap<QString, QStringList> nDeps;
		for (PackagePointer p : package->recursiveDependencies()) {
			for (const QString &mngr : p->nativeDependencies().keys()) {
				nDeps[mngr].append(p->nativeDependencies()[mngr]);
				nDeps[mngr].removeDuplicates();
			}
		}

		for (const QString &mngr : nDeps.keys()) {
			out << "    " << mngr << ": " << nDeps[mngr].join(", ") << endl;
		}
	} else {
		// only output packages
		const QString mngr = parser->value("nativeDependencies");
		QStringList nDeps;
		for (PackagePointer p : package->recursiveDependencies()) {
			nDeps.append(p->nativeDependencies()[mngr]);
		}
		out << nDeps.join(' ') << endl;
	}

	return true;
}
