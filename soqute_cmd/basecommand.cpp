#include "basecommand.h"

#include <QDebug>
#include <QCommandLineParser>

#include "configurationhandler.h"
#include "util_cmd.h"

BaseCommand::BaseCommand(ConfigurationHandler *configHandler, PackageList *packages,
						 QObject *parent)
	: QObject(parent), configHandler(configHandler), packages(packages)
{
	parser = new QCommandLineParser();
}

void BaseCommand::showHelp()
{
	setupParser();
	parser->showHelp();
}

bool BaseCommand::preMetadataLoad(const QStringList &args)
{
	setupParser();
	parser->parse(args);
	Util::handleCommonArguments(parser);

	return preMetadataLoadImplementation();
}

int BaseCommand::execute()
{
	if (executeImplementation()) {
		return 0;
	} else {
		return m_explicitExitCode;
	}
}
