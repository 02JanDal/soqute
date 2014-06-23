#include "searchcommand.h"

#include <QCommandLineParser>

#include "package.h"
#include "configurationhandler.h"
#include "installedpackages.h"
#include "textstream.h"
#include "util_core.h"

SearchCommand::SearchCommand(ConfigurationHandler *configHandler, PackageList *packages,
							 QObject *parent)
	: BaseCommand(configHandler, packages, parent)
{
}

void SearchCommand::setupParser()
{
	parser = new QCommandLineParser();
	parser->setApplicationDescription(tr("Searches for packages"));
	parser->addVersionOption();
	parser->addHelpOption();
	parser->addPositionalArgument(tr("query"), tr("The queries used to search for packages"),
								  tr("<query>*"));
	parser->addOption(
		QCommandLineOption(QStringList() << "d"
										 << "matchDescription",
						   tr("If specified, the description will be search, too")));
	parser->addOption(QCommandLineOption("host", tr("Show packages for the <platform> instead of the current platform"), tr("platform"), Util::currentPlatform()));
}

QString padded(const QString &string, const int size, const QChar character = QChar(' '))
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
	if (parser->positionalArguments().isEmpty()) {
		out << "You need to specify at least one query" << endl;
		return false;
	}

	const QList<PackagePointer> pkgs = packages->findPackages(
		parser->positionalArguments(), parser->isSet("matchDescription"));

	if (pkgs.isEmpty()) {
		out << "No packages where found" << endl;
		return false;
	}

	// extract the metadata we're interested in and fill metadataMapping with it
	QMap<QPair<QString, QString>, QPair<QString, QStringList>> metadataMapping;
	for (auto pkg : pkgs) {
		if (pkg->host().isEmpty() || pkg->host() == parser->value("host"))
		{
			auto key = qMakePair(pkg->id(), pkg->target());
			auto existing = metadataMapping[key];
			existing.first = pkg->description(); // in case we don't have it yet
			existing.second.append(pkg->version());
			metadataMapping[key] = existing;
		}
	}

	// calculate lengths so we know how much we need to pad
	int longestId = 0;
	int longestVersions = 0;
	int longestTarget = 0;
	for (auto it = metadataMapping.begin(); it != metadataMapping.end(); ++it) {
		it.value().second.removeDuplicates();
		longestId = qMax(longestId, it.key().first.length());
		longestVersions = qMax(longestVersions, it.value().second.join(", ").length());
		longestTarget = qMax(longestTarget, it.key().second.length());
	}

	// we use a map here to sort the results by platform
	QMultiMap<QString, QString> rows;
	rows.insert("", QString("\E[1m I  ") + padded("ID", longestId) + "  " +
						padded("Versions", longestVersions) + "  " +
						padded("Target", longestTarget) + "  Description\E[0m");
	for (auto it = metadataMapping.begin(); it != metadataMapping.end(); ++it) {
		const QString id = it.key().first;
		const QString target = it.key().second;
		const QString description = it.value().first;
		const QString version = it.value().second.join(", ");
		QString str = QString("  ") + padded(id, longestId) + "  " +
					  padded(version, longestVersions) + "  " +
					  padded(target, longestTarget) + "  " + description.split('\n')[0];
		if (ConfigurationHandler::instance()->installedPackages()->isPackageInstalled(
				id, QString(), QString(), target)) {
			str = " i" + str;
		} else {
			str = "  " + str;
		}
		rows.insert(target, str);
	}

	for (const QString &row : rows) {
		out << row << endl;
	}

	return true;
}
