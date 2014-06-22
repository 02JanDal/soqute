#include <QCoreApplication>
#include <QStringList>
#include <QDebug>
#include <QCommandLineParser>

#include "loadmetadata.h"
#include "package.h"
#include "configurationhandler.h"
#include "util_cmd.h"

#include "searchcommand.h"
#include "configcommand.h"
#include "showcommand.h"
#include "installcommand.h"
#include "removecommand.h"
#include "textstream.h"

QCommandLineParser *createGeneralParser(const QString &programName)
{
	QCommandLineParser *parser = new QCommandLineParser;
	parser->setApplicationDescription(QObject::tr(
		"Searches for, installs and removes Qt related stuff. You may want to try \"%1 help\"")
										  .arg(programName));
	parser->addHelpOption();
	parser->addVersionOption();
	parser->addPositionalArgument(QObject::tr("command"), QObject::tr("The command to execute"),
								  QObject::tr("[<command>]"));

	return parser;
}

int handleHelpCommand(const QString &programName, const QStringList &arguments,
					  const QMap<QString, BaseCommand *> commands)
{
	QCommandLineParser helpParser;
	helpParser.setApplicationDescription(QObject::tr(
		"Searches for, installs and removes Qt related stuff. You may want to try \"%1 help\"")
											 .arg(programName));
	helpParser.addHelpOption();
	helpParser.addVersionOption();
	helpParser.addPositionalArgument(QObject::tr("command"),
									 QObject::tr("The command to execute"),
									 QObject::tr("[<command>]"));
	helpParser.parse(arguments);
	Util::handleCommonArguments(&helpParser);
	if (helpParser.positionalArguments().isEmpty() ||
		helpParser.positionalArguments().first() == "help") {
		out << helpParser.helpText() << endl
			<< "Choose from the following for <command>:" << endl << "\t"
			<< QStringList(commands.keys()).join("\n\t") << endl;
	} else {
		const QString command = helpParser.positionalArguments().first();
		if (commands.contains(command)) {
			commands[command]->showHelp();
		} else {
			out << "\nUnknown command. Choose from the following:\n" << flush;
			out << "\t" << QStringList(commands.keys()).join("\n\t") << "\n" << flush;
			return -1;
		}
	}
	return 0;
}

QMap<QString, BaseCommand *> setupCommands(ConfigurationHandler *configHandler,
										   PackageList *packages, QObject *root)
{
	QMap<QString, BaseCommand *> commands;
	commands.insert("show", new ShowCommand(configHandler, packages, root));
	commands.insert("search", new SearchCommand(configHandler, packages, root));
	commands.insert("config", new ConfigCommand(configHandler, packages, root));
	commands.insert("install", new InstallCommand(configHandler, packages, root));
	commands.insert("remove", new RemoveCommand(configHandler, packages, root));
	return commands;
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	a.setOrganizationName("JanDalheimer");
	a.setApplicationName("soqute");
	a.setApplicationVersion("0.0.1");

	QObject *root = new QObject();
	PackageList *packages = new PackageList(root);
	ConfigurationHandler *configHandler = new ConfigurationHandler(root);
	LoadMetadata *metadataLoader = new LoadMetadata(packages, root);
	metadataLoader->setRepositoryList(configHandler->repositoryUrls());

	QMap<QString, BaseCommand *> commands = setupCommands(configHandler, packages, root);

	const QStringList arguments = a.arguments();

	if (arguments.size() <= 1) {
		out << "No command given. Try \"" << arguments.first() << " help\"" << endl;
		return -1;
	}

	const QString command = arguments.at(1);
	if (command == "help") {
		createGeneralParser(arguments.first())->showHelp();
	}

	// command doesn't exist. notify user, show help and exit
	if (!commands.contains(command)) {
		out << "Unknown command \"" << command << "\".\n" << flush;
		createGeneralParser(arguments.first())->showHelp(-1);
	}

	BaseCommand *cmd = commands[command];

	// load metadata
	if (cmd->needMetadata()) {
		auto checkAndOutputMessages = [&metadataLoader]()
		{
			while (metadataLoader->hasMessage()) {
				QPair<LoadMetadata::Message, QVariant> msg = metadataLoader->takeLastMessage();
				out << metadataLoader->messageToString(msg.first, msg.second) << "\n" << flush;
			}
		};

		metadataLoader->loadMetadata();
		while (!metadataLoader->isDone()) {
			qApp->processEvents();
			checkAndOutputMessages();
		}
		checkAndOutputMessages();
		if (!metadataLoader->isSuccess()) {
			return 1;
		}
	}

	if (!cmd->execute(arguments.mid(1))) {
		out << flush;
		return 1;
	}
	out << flush;

	return 0;
}
