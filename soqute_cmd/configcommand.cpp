#include "configcommand.h"

#include <QtCore/QDebug>
#include <iostream>

#include "configurationhandler.h"
#include "qcommandlineparser.h"
#include "util_cmd.h"

// TODO use Qt's property system to get rid of function pointers?

enum Operation {
    None = 0x00,
    Set  = 0x01,
    Read = 0x02,
    Delete = 0x04,
    Add  = 0x06
};
Q_DECLARE_FLAGS(Operations, Operation)
Q_DECLARE_OPERATORS_FOR_FLAGS(Operations)

// TODO move anonym stuff to ConfigCommand

typedef void(*ReadSettingFunctionPtr)(const ConfigurationHandler*);
typedef QString(ConfigurationHandler::*ReadDefaultSettingFunctionPtr)()const;
typedef void(*AddSettingFunctionPtr)(const QStringList&, ConfigurationHandler*);
typedef void(*DeleteSettingFunctionPtr)(const QString&, ConfigurationHandler*);
typedef void(ConfigurationHandler::*SetSettingFunctionPtr)(const QString&);

void readRepositories(const ConfigurationHandler* configHandler);
void addRepositories(const QStringList& repos, ConfigurationHandler* configHandler);
void deleteRepository(const QString& value, ConfigurationHandler* configHandler);

/**
 * @brief The Setting struct contains data for settings, like supported operations, the read function etc.
 *
 * There are several constructors to make it easier to setup this struct
 *
 * You always have to specify which operations are supported, normally you always want atleast read and either add or set.
 * You also have to specify either a ReadSettingFunctionPtr or ReadDefaultSettingFunctionPtr. The first one will call any
 *   free standing function with the ConfigurationHandler as argument, the later will call a member function of ConfigurationHandler
 *   which should return the value of the function as a QString. Using the later will output something like "key: value". Use
 *   the first one if you need some custom output, for example when using a list of items.
 * You might also want to either specify a setter or adder function. The add function can be any free standing function and will
 *   take a list of the values to add and a ConfigurationHandler object. The set function should be a member function of
 *   ConfigurationHandler and take a QString containing the value to set.
 */
struct Setting
{
    Setting() {}
    Setting(Operations ops, ReadDefaultSettingFunctionPtr readFunc) : supportedOperations(ops), readDefaultFunction(readFunc) {}
    Setting(Operations ops, ReadSettingFunctionPtr readFunc) : supportedOperations(ops), readFunction(readFunc) {}
    Setting(Operations ops, ReadSettingFunctionPtr readFunc, SetSettingFunctionPtr setFunc) : supportedOperations(ops), readFunction(readFunc), readDefaultFunction(0), setFunction(setFunc) {}
    Setting(Operations ops, ReadDefaultSettingFunctionPtr readFunc, SetSettingFunctionPtr setFunc) : supportedOperations(ops), readFunction(0), readDefaultFunction(readFunc), setFunction(setFunc) {}
    Setting(Operations ops, ReadSettingFunctionPtr readFunc, AddSettingFunctionPtr addFunc, SetSettingFunctionPtr setFunc) : supportedOperations(ops), readFunction(readFunc), readDefaultFunction(0), addFunction(addFunc), setFunction(setFunc) {}
    Setting(Operations ops, ReadSettingFunctionPtr readFunc, AddSettingFunctionPtr addFunc, DeleteSettingFunctionPtr deleteFunc) : supportedOperations(ops), readFunction(readFunc), readDefaultFunction(0), addFunction(addFunc), deleteFunction(deleteFunc) {}
    Operations supportedOperations;
    ReadSettingFunctionPtr readFunction;
    ReadDefaultSettingFunctionPtr readDefaultFunction;
    AddSettingFunctionPtr addFunction;
    DeleteSettingFunctionPtr deleteFunction;
    /// This should point at a member function of ConfigurationHandler
    SetSettingFunctionPtr setFunction;
};

bool operator==(const Setting& s1, const Setting& s2)
{
    return s1.readFunction == s2.readFunction && s1.readDefaultFunction == s2.readDefaultFunction && s1.supportedOperations == s2.supportedOperations;
}

/// \returns A map with available setting keys mapped to thier Setting object
QMap<QString, Setting> availableSettings()
{
    QMap<QString, Setting> settings;
    settings.insert("repositories", Setting(Read | Add | Delete, &readRepositories, &addRepositories, &deleteRepository));
    settings.insert("package-manager", Setting(Read | Set, &ConfigurationHandler::packageManager, &ConfigurationHandler::setPackageManager));
	settings.insert("install-prefix", Setting(Read | Set, &ConfigurationHandler::installRoot, &ConfigurationHandler::setInstallRoot));
    return settings;
}

void readRepositories(const ConfigurationHandler* configHandler)
{
    std::cout << "Repositories:" << std::endl;
    const QList<QUrl> repositories = configHandler->repositoryUrls();
    foreach (const QUrl& repository, repositories) {
        std::cout << "  " << qPrintable(repository.toString()) << std::endl;
    }
}
void addRepositories(const QStringList& repos, ConfigurationHandler* configHandler)
{
    foreach (const QString& repo, repos) {
        configHandler->addRepositoryUrl(QUrl::fromUserInput(repo));
    }
}
void deleteRepository(const QString& value, ConfigurationHandler* configHandler)
{
    configHandler->removeRepositoryUrl(QUrl(value));
}

ConfigCommand::ConfigCommand(ConfigurationHandler *configHandler, PackageList* packages, QObject *parent) :
	BaseCommand(configHandler, packages, parent)
{
}

void ConfigCommand::setupParser()
{
    parser = new QCommandLineParser();
	parser->setApplicationDescription(tr("Gives access to settings"));
	parser->addVersionOption();
	parser->addHelpOption();
	parser->addPositionalArgument(tr("key"), tr("The key of the value to set"), tr("<key>"));
	parser->addPositionalArgument(tr("values"), tr("The values to set"), tr("<value>*"));
    parser->addOption(QCommandLineOption(QStringList() << "s" << "set", tr("Sets value of key to the specified value")));
    parser->addOption(QCommandLineOption(QStringList() << "r" << "read",
                                         tr("Reads the value associated with the specified key (this is the default if no other option is given). You may use \"all\" as key to display all settings")));
    parser->addOption(QCommandLineOption(QStringList() << "a" << "add", tr("Adds all values following key to key")));
    parser->addOption(QCommandLineOption(QStringList() << "d" << "delete", tr("Deletes a value from a key")));
    parser->addOption(QCommandLineOption(QStringList() << "l" << "list", tr("Lists all known keys")));
}

bool ConfigCommand::executeImplementation()
{
    Operation operation;
    if (parser->isSet("set")) {
        operation = Set;
    } else if (parser->isSet("read")) {
        operation = Read;
    } else if (parser->isSet("add")) {
        operation = Add;
    } else if (parser->isSet("delete")) {
        operation = Delete;
    } else if (parser->isSet("list")) {
        std::cout << "Known keys: " << qPrintable(QStringList(availableSettings().keys()).join(", ")) << std::endl;
		return false;
    }

	QStringList remainingArgs = parser->positionalArguments();
    if (remainingArgs.isEmpty()) {
        std::cout << "You need so specify a key" << std::endl;
		return false;
    }
    const QString key = remainingArgs.takeFirst();

    QMap<QString, Setting> settings = availableSettings();

    if (!settings.contains(key) && !(operation == Read && key == "all")) {
        std::cout << "Unknown key" << std::endl;
		return false;
    }

	if ((operation == Set || operation == Add || operation == Delete) && parser->positionalArguments().isEmpty()) {
        std::cout << "You need so specify (a) value(s)" << std::endl;
		return false;
    }

    if (key == "all") {
        foreach (Setting setting, settings.values()) {
            if (setting.readFunction == 0) {
                std::cout << qPrintable(settings.key(setting)) << ": " << qPrintable((configHandler->*setting.readDefaultFunction)()) << std::endl;
            } else {
                (*setting.readFunction)(configHandler);
            }
        }
        return true;
    }

    Setting setting = settings[key];

    if (setting.supportedOperations.testFlag(operation)) {
        if (operation == Set) {
            (configHandler->*setting.setFunction)(remainingArgs.first());
            std::cout << "set " << qPrintable(key) << " to " << qPrintable(remainingArgs.first()) << std::endl;
        } else if (operation == Add) {
            std::cout << "Adding values to " << qPrintable(key) << std::endl;
            (*setting.addFunction)(remainingArgs, configHandler);
        } else if (operation == Read) {
            if (setting.readFunction == 0) {
                std::cout << qPrintable(key) << ": " << qPrintable((configHandler->*setting.readDefaultFunction)()) << std::endl;
            } else {
                (*setting.readFunction)(configHandler);
            }
        } else if (operation == Delete) {
            std::cout << "Removing " << qPrintable(remainingArgs[0]) << " from " << qPrintable(key) << std::endl;
            (*setting.deleteFunction)(remainingArgs[0], configHandler);
        }
    } else {
        std::cout << "The key \"" << qPrintable(key) << "\" does not support this operation" << std::endl;
		return false;
    }

	return true;
}
