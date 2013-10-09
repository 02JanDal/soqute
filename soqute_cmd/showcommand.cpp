#include "showcommand.h"

#include <QCoreApplication>
#include <QString>
#include <QRegularExpression>
#include <QDebug>

#include "taskhandler.h"
#include "util_cmd.h"
#include "configurationhandler.h"
#include "qcommandlineparser.h"
#include "package.h"
#include "dependencycalculator.h"
#include "installedpackages.h"
#include "textstream.h"

ShowCommand::ShowCommand(ConfigurationHandler *configHandler, PackageList* packages, QObject *parent) :
	BaseCommand(configHandler, packages, parent)
{
}

void ShowCommand::setupParser()
{
    parser = new QCommandLineParser();
    parser->addVersionOption();
    parser->addHelpOption(tr("Shows information about a package"));
	parser->setRemainingArgumentsHelpText(tr("<package-name>[/<version>][#<platform>]"));
    parser->addOption(QCommandLineOption(QStringList() << "nativeDependencies", tr("Only outputs the native packages for piping to the package manager"),
                                         tr("package-manager"), configHandler->packageManager()));
}

bool ShowCommand::executeImplementation()
{
	if (parser->remainingArguments().isEmpty()) {
		out << "You need to specify a package\n" << flush;
		return false;
    }

	// captures expressions like this: <name>[/<version>][#<platform>]
	// examples: qtbase/5.0.0#win32-g++
    //           qtjsbackend
	//           qtpim#linux-g++-32
	//           qtquick1/5.1.0
	QRegularExpression exp("([a-z0-9\\-_\\+]*)(/[a-z0-9\\-\\+\\.]*)?(#[a-z0-9\\-\\+]*)?");
    QRegularExpressionMatch match = exp.match(parser->remainingArguments()[0]);
	const QString id = match.captured(1);
	const QString version = match.captured(2).remove(0, 1);
	const QString platform = match.captured(3).remove(0, 1);

	const Package* package = packages->package(id, version, platform);

	if (package == 0) {
		out << "No such package found\n" << flush;
		return false;
	}

	if (!parser->isSet("nativeDependencies")) {
		// populate the other versions/platforms lists
		const QList<const Package*> others = packages->otherPackages(package);
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
		out << "Status:              "
			<< (ConfigurationHandler::instance()->installedPackages()->isPackageInstalled(package->id(), package->version(), package->platform())
			   ? "Installed" : "Not installed")
				 << "\n" << flush;
		out << "ID:                  " << package->id() << "\n"
			<< "Description:         " << package->description() << "\n"
			<< "Version:             " << package->version() << "\n"
			<< "Other versions:      " << allVersions.join(", ") << "\n"
			<< "Platform:            " << package->platform() << "\n"
			<< "Other platforms:     " << allPlatforms.join(", ") << "\n"
			<< "Dependencies:        " << flush;
        foreach (const PackagePointer p, package->recursiveDependencies()) {
			out << p->id() << " ";
		}
		out << "\n"
			<< "Native dependencies: \n" << flush;
		QMap<QString, QStringList> nDeps;
        foreach (const PackagePointer p, package->recursiveDependencies()) {
			foreach (const QString& mngr, p->nativeDependencies().keys()) {
				nDeps[mngr].append(p->nativeDependencies()[mngr]);
                nDeps[mngr].removeDuplicates();
			}
		}

		foreach (const QString& mngr, nDeps.keys()) {
			out << "    " << mngr << ": " << nDeps[mngr].join(", ") << "\n" << flush;
		}
	} else {
		// only output packages
		QMap<QString, QStringList> nDeps;
        foreach (const PackagePointer p, package->recursiveDependencies()) {
			foreach (const QString& mngr, p->nativeDependencies().keys()) {
				nDeps[mngr].append(p->nativeDependencies()[mngr]);
			}
		}
		QStringList packages = nDeps[parser->value("nativeDependencies")];
		out << packages.join(' ') << "\n" << flush;
	}

	return true;
}
