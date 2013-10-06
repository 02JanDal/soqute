#include "searchcommand.h"

#include "qcommandlineparser.h"
#include "package.h"
#include "configurationhandler.h"
#include "installedpackages.h"
#include "textstream.h"

SearchCommand::SearchCommand(ConfigurationHandler *configHandler, PackageList *packages, QObject *parent) :
	BaseCommand(configHandler, packages, parent)
{
}

void SearchCommand::setupParser()
{
    parser = new QCommandLineParser();
    parser->addVersionOption();
    parser->addHelpOption(tr("Searches for packages"));
    parser->setRemainingArgumentsHelpText(tr("<query>*"));
    parser->addOption(QCommandLineOption(QStringList() << "d" << "matchDescription", tr("If specified, the description will be search, too")));
}

QString padded(const QString& string, const int size, const QChar character = QChar(' '))
{
	if (string.size() >= size) {
		return string;
	}
	QString padding;
	padding.fill(character, size - string.size());
	return string + padding;
}

bool SearchCommand::executeImplementation()
{
	if (parser->remainingArguments().isEmpty()) {
		out << "You need to specify at least one query\n" << flush;
		return false;
	}

	const QList<const Package*> pkgs = packages->findPackages(parser->remainingArguments(), parser->isSet("matchDescription"));

	if (pkgs.isEmpty()) {
		out << "No packages where found\n" << flush;
		return false;
	}

	int longestId = 0;
	int longestPlatform = 0;

    foreach (const  Package* package, pkgs) {
        longestId = qMax(longestId, package->id().length());
		longestPlatform = qMax(longestPlatform, package->platform().length());
	}

	// we use a map here to sort the results
	QMultiMap<QString, QString> rows;
	rows.insert("", QString("\E[1m I  ") + padded("ID", longestId) + "  " + padded("Platform", longestPlatform) + "  Description\E[0m");
	foreach (const Package* package, pkgs) {
		QString str = QString("  ") + padded(package->id(), longestId) + "  " + padded(package->platform(), longestPlatform) + "  " + package->description().split('\n')[0];
		if (ConfigurationHandler::instance()->installedPackages()->isPackageInstalled(package->id(), package->version(), package->platform())) {
			str = " i" + str;
		} else {
			str = "  " + str;
		}
		rows.insert(package->platform(), str);
	}

	foreach (const QString& row, rows) {
		out << row << "\n" << flush;
	}

	return true;
}
