#include "showcommand.h"

#include <QCoreApplication>
#include <QString>
#include <QRegularExpression>
#include <QDebug>
#include <QCommandLineParser>

#include "util_cmd.h"
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
								  tr("<package-name>[/<version>][#<platform>]"));
	parser->addOption(QCommandLineOption(
		QStringList() << "nativeDependencies",
		tr("Only outputs the native packages for piping to the package manager"),
		tr("package-manager"), configHandler->packageManager()));
}

bool ShowCommand::executeImplementation()
{
	if (parser->positionalArguments().isEmpty()) {
		out << "You need to specify a package" << endl;
		return false;
	}

	// captures expressions like this: <name>[/<version>][#<platform>]
	// examples: qtbase/5.0.0#win32-g++
	//           qtjsbackend
	//           qtpim#linux-g++-32
	//           qtquick1/5.1.0
	QRegularExpression exp("([a-z0-9\\-_\\+]*)(/[a-z0-9\\-\\+\\.]*)?(#[a-z0-9\\-\\+]*)?");
	QRegularExpressionMatch match = exp.match(parser->positionalArguments()[0]);
	const QString id = match.captured(1);
	const QString version = match.captured(2).remove(0, 1);
	const QString platform = match.captured(3).remove(0, 1);

	const Package *package = packages->package(id, version, platform);

	if (package == 0) {
		out << "No such package found" << endl;
		return false;
	}

	if (!parser->isSet("nativeDependencies")) {
		// populate the other versions/platforms lists
		const QList<const Package *> others = packages->otherPackages(package);
		QStringList allPlatforms;
		QStringList allVersions;
		for (PackagePointer pkg : others) {
			allPlatforms.append(pkg->platform());
			allVersions.append(pkg->version());
		}
		allPlatforms.removeDuplicates();
		allPlatforms.removeAll(package->platform());
		allVersions.removeDuplicates();
		allVersions.removeAll(package->version());

		// just output everything
		out << "Status:              "
			<< (ConfigurationHandler::instance()->installedPackages()->isPackageInstalled(
					package->id(), package->version(), package->platform())
					? "Installed"
					: "Not installed") << endl;
		out << "ID:                  " << package->id() << endl
			<< "Description:         " << package->description() << endl
			<< "Version:             " << package->version() << endl
			<< "Other versions:      " << allVersions.join(", ") << endl
			<< "Platform:            " << package->platform() << endl
			<< "Other platforms:     " << allPlatforms.join(", ") << endl
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
