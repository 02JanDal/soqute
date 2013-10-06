#include <QCoreApplication>
#include <QStringList>
#include <QDebug>

#include "loadmetadata.h"
#include "package.h"
#include "configurationhandler.h"
#include "qcommandlineparser.h"
#include "util_cmd.h"

#include "searchcommand.h"
#include "configcommand.h"
#include "showcommand.h"
#include "installcommand.h"
#include "removecommand.h"
#include "textstream.h"

QStringList parseCommandLineArguments(const QStringList& arguments, const bool showHelpAndExit = false)
{
    QCommandLineParser parser;
	parser.addHelpOption(QObject::tr("Searches for, installs and removes Qt related stuff. You may want to try \"%1 help\"").arg(arguments.first()));
    parser.addVersionOption();
    parser.setRemainingArgumentsHelpText(QObject::tr("[<command>]"));

    if (showHelpAndExit) {
        parser.showHelp();
        exit(0);
    }

    parser.parse(arguments);
    Util::handleCommonArguments(&parser);

    if (parser.remainingArguments().isEmpty()) {
		out << "No command given\n\n" << flush;
        parser.showHelp();
        exit(-1);
    }

    return parser.remainingArguments();
}

int handleHelpCommand(const QStringList& arguments, const QMap<QString, BaseCommand*> commands)
{
    QCommandLineParser helpParser;
    helpParser.addHelpOption(QObject::tr("Shows help for %1").arg(qApp->applicationName()));
    helpParser.addVersionOption();
    helpParser.setRemainingArgumentsHelpText(QObject::tr("[<command>]"));
    helpParser.parse(arguments);
    Util::handleCommonArguments(&helpParser);
    if (helpParser.remainingArguments().isEmpty()) {
        helpParser.showHelp();
		out << "\nChoose from the following for <command>:\n" << flush;
		out << "\t" << qPrintable(QStringList(commands.keys()).join("\n\t")) << "\n" << flush;
    } else {
        const QString command = helpParser.remainingArguments().first();
        if (command == "help") {
            helpParser.showHelp();
			out << "\nChoose from the following for <command>:\n" << flush;
			out << "\t" << QStringList(commands.keys()).join("\n\t") << "\n" << flush;
        } else if (commands.contains(command)) {
            commands[command]->showHelp();
        } else {
			out << "\nUnknown command. Choose from the following:\n" << flush;
			out << "\t" << QStringList(commands.keys()).join("\n\t") << "\n" << flush;
            return -1;
        }
    }
    return 0;
}

QMap<QString, BaseCommand*> setupCommands(ConfigurationHandler* configHandler, PackageList* packages, QObject* root)
{
    QMap<QString, BaseCommand*> commands;
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

    QObject* root = new QObject();
	PackageList* packages = new PackageList(root);
	ConfigurationHandler* configHandler = new ConfigurationHandler(root);
	LoadMetadata* metadataLoader = new LoadMetadata(packages, root);
	metadataLoader->setRepositoryList(configHandler->repositoryUrls());

    QMap<QString, BaseCommand*> commands = setupCommands(configHandler, packages, root);

    QStringList arguments = parseCommandLineArguments(a.arguments());
    if (arguments.first() == "help") {
        return handleHelpCommand(arguments, commands);
    }

    const QString command = arguments.first();
    // command doesn't exist. notify user, show help and exit
    if (!commands.contains(command)) {
        out << "Unknown command \"" << command << "\".\n" << flush;
        parseCommandLineArguments(QStringList() << a.arguments().first(), true);
        return -1;
    }

    BaseCommand* cmd = commands[command];

	// load metadata
    if (cmd->needMetadata()) {
		metadataLoader->downloadMetadata();
		while (!metadataLoader->isDone()) {
			qApp->processEvents();
			if (metadataLoader->hasMessage()) {
				QPair<LoadMetadata::Message, QVariant> msg = metadataLoader->takeLastMessage();
				out << metadataLoader->messageToString(msg.first, msg.second) << "\n" << flush;
			}
		}
	}

    if (!cmd->execute(arguments)) {
		return 1;
	}
    
	return 0;
}
