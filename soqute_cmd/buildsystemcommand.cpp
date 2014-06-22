#include "buildsystemcommand.h"

#include <QCommandLineParser>
#include <QDebug>

#include "textstream.h"
#include "configurationhandler.h"
#include "exception.h"
#include "util_core.h"
#include "util_cmd.h"

BuildsystemCommand::BuildsystemCommand(ConfigurationHandler *configHandler,
									   PackageList *packages, QObject *parent)
	: InstallCommandBase(configHandler, packages, parent)
{
}

void BuildsystemCommand::setupParser()
{
	parser = new QCommandLineParser();
	parser->setApplicationDescription(tr("FOR INTERNAL USE ONLY!!!"));
	parser->addHelpOption();
	parser->addOption(QCommandLineOption(
		"defaultplatform", tr("Default platform if no other is given. Empty means current."),
		tr("platform"), Util::currentPlatform()));
	parser->addOption(QCommandLineOption(
		"defaultversion", tr("Default version if no other is given. Empty means latest."),
		tr("version"), ""));
	parser->addOption(QCommandLineOption(
		"repository", tr("Extra repository to add for this run"), tr("repo")));
	parser->addOption(QCommandLineOption(
		"outdir", tr("For testing. Overrides the installation directory for this run."),
		tr("directory")));
	parser->addPositionalArgument(tr("package identifiers"),
								  tr("Identifiers for all packages to install"),
								  tr("<package-name>[/<version>][#<platform>]*"));
}

bool BuildsystemCommand::preMetadataLoadImplementation()
{
	if (!configHandler->allowBuildSystemInstall()) {
		setExplicitExitCode(2);
		return false;
	}

	const QString repo = parser->value("repository").remove('"');
	if (parser->isSet("repository") && !repo.isEmpty()) {
		configHandler->setTemporaryExtraRepository(
			QUrl::fromUserInput(repo));
	}
	const QString outdir = parser->value("outdir").remove('"');
	if (parser->isSet("outdir") && !outdir.isEmpty()) {
		configHandler->setTemporaryInstallRoot(outdir);
	}

	return true;
}

bool BuildsystemCommand::executeImplementation()
{
	try
	{
		baseChecks();

		constructPackageList(parser->positionalArguments());

		if (m_pkgs.isEmpty()) {
			err << Util::niceJoin(parser->positionalArguments());
			return true;
		}

		installPackages(new FailAuthenticator);
	}
	catch (Exception &e)
	{
		out << "Error: " << e.message() << endl;
		return false;
	}
	QStringList packages;
	for (PackagePointer pkg : m_pkgs) {
		packages.append(pkg->id());
	}
	err << Util::niceJoin(packages);
	return true;
}
