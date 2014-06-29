#include "configcommand.h"

#include <QDebug>
#include <QCommandLineParser>

#include "configurationhandler.h"
#include "util_cmd.h"
#include "textstream.h"

// TODO use Qt's property system?

enum Operation {
	None = 0x00,
	Set = 0x01,
	Read = 0x02,
	Delete = 0x04,
	Add = 0x08
};
Q_DECLARE_FLAGS(Operations, Operation)
Q_DECLARE_OPERATORS_FOR_FLAGS(Operations)

struct Setting
{
	Setting(const QString &key, Operations ops = None) : key(key), supportedOperations(ops)
	{
	}

	QString key;
	Operations supportedOperations;

	virtual QStringList read(ConfigurationHandler *)
	{
		return QStringList();
	}
	virtual void add(const QStringList &, ConfigurationHandler *)
	{
	}
	virtual void deleteValue(const QString &, ConfigurationHandler *)
	{
	}
	virtual bool set(const QString &, ConfigurationHandler *)
	{
		return false;
	}
};
struct RepositoriesSetting : public Setting
{
	RepositoriesSetting() : Setting("repositories", Read | Add | Delete)
	{
	}

	QStringList read(ConfigurationHandler *handler) override
	{
		QStringList out;
		for (const auto url : handler->repositoryUrls()) {
			out.append(url.toString());
		}
		return out;
	}
	void add(const QStringList &values, ConfigurationHandler *handler) override
	{
		for (const auto url : values) {
			handler->addRepositoryUrl(QUrl::fromUserInput(url));
		}
	}
	void deleteValue(const QString &value, ConfigurationHandler *handler) override
	{
		handler->removeRepositoryUrl(QUrl::fromUserInput(value));
	}
};
struct PackageManagerSetting : public Setting
{
	PackageManagerSetting() : Setting("package-manager", Read | Set)
	{
	}

	QStringList read(ConfigurationHandler *handler) override
	{
		return QStringList() << handler->packageManager();
	}
	bool set(const QString &value, ConfigurationHandler *handler) override
	{
		handler->setPackageManager(value);
		return true;
	}
};
struct InstallPrefixSetting : public Setting
{
	InstallPrefixSetting() : Setting("install-prefix", Read | Set)
	{
	}

	QStringList read(ConfigurationHandler *handler) override
	{
		return QStringList() << handler->installRoot();
	}
	bool set(const QString &value, ConfigurationHandler *handler) override
	{
		handler->setInstallRoot(value);
		return true;
	}
};
struct AllowBuildSystemInstallSetting : public Setting
{
	AllowBuildSystemInstallSetting() : Setting("allow-build-system-install", Read | Set)
	{
	}

	QStringList read(ConfigurationHandler *handler) override
	{
		return QStringList() << (handler->allowBuildSystemInstall() ? "true" : "false");
	}
	bool set(const QString &value, ConfigurationHandler *handler) override
	{
		if (value == "true") {
			handler->setAllowBuildSystemInstall(true);
		} else if (value == "false") {
			handler->setAllowBuildSystemInstall(false);
		} else {
			return false;
		}
		return true;
	}
};

/// \returns A map with available setting keys mapped to thier Setting object
QList<Setting *> availableSettings()
{
	QList<Setting *> settings;
	settings.append(new RepositoriesSetting());
	settings.append(new PackageManagerSetting());
	settings.append(new InstallPrefixSetting());
	settings.append(new AllowBuildSystemInstallSetting());
	return settings;
}

ConfigCommand::ConfigCommand(ConfigurationHandler *configHandler, PackageList *packages,
							 QObject *parent)
	: BaseCommand(configHandler, packages, parent)
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
	parser->addOption(QCommandLineOption(QStringList() << "s"
													   << "set",
										 tr("Sets value of key to the specified value")));
	parser->addOption(QCommandLineOption(
		QStringList() << "r"
					  << "read",
		tr("Reads the value associated with the specified key (this is the default if no other "
		   "option is given). You may use \"all\" as key to display all settings")));
	parser->addOption(QCommandLineOption(QStringList() << "a"
													   << "add",
										 tr("Adds all values following key to key")));
	parser->addOption(QCommandLineOption(QStringList() << "d"
													   << "delete",
										 tr("Deletes a value from a key")));
	parser->addOption(QCommandLineOption(QStringList() << "l"
													   << "list",
										 tr("Lists all known keys")));
}

bool ConfigCommand::executeImplementation()
{
	out.setFieldAlignment(QTextStream::AlignLeft);

	QList<Setting *> settings = availableSettings();

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
		QStringList keys;
		std::for_each(settings.begin(), settings.end(),
					  [&keys](const Setting *setting) { keys.append(setting->key); });
		out << "Known keys: " << qPrintable(keys.join(", ")) << endl;
		return true;
	}

	QStringList remainingArgs = parser->positionalArguments();
	if (remainingArgs.isEmpty()) {
		out << "You need so specify a key" << endl;
		return false;
	}
	const QString key = remainingArgs.takeFirst();

	auto selectedSettingIterator =
		std::find_if(settings.begin(), settings.end(),
					 [key](const Setting *setting) { return setting->key == key; });

	if (selectedSettingIterator == settings.end() && !(operation == Read && key == "all")) {
		out << "Unknown key" << endl;
		return false;
	}

	if ((operation == Set || operation == Add || operation == Delete) &&
		parser->positionalArguments().isEmpty()) {
		out << "You need so specify (a) value(s)" << endl;
		return false;
	}

	if (key == "all" && operation == Read) {
		for (auto setting : settings) {
			if (setting->supportedOperations.testFlag(Read)) {
				out << qSetFieldWidth(20) << setting->key
					<< (" : " + setting->read(configHandler).join(", ")) << qSetFieldWidth(0)
					<< endl;
			}
		}
		return true;
	}

	Setting *setting = *selectedSettingIterator;

	if (setting->supportedOperations.testFlag(operation)) {
		if (operation == Set) {
			if (!setting->set(remainingArgs.first(), configHandler)) {
				out << "Invalid value for " << key << endl;
			} else {
				out << "Setting " << qPrintable(key) << " to "
					<< qPrintable(remainingArgs.first()) << endl;
			}
		} else if (operation == Add) {
			out << "Adding values to " << qPrintable(key) << endl;
			setting->add(remainingArgs, configHandler);
		} else if (operation == Read) {
			out << setting->key << " : " << setting->read(configHandler).join(", ") << endl;
		} else if (operation == Delete) {
			out << "Removing " << qPrintable(remainingArgs[0]) << " from " << qPrintable(key)
				<< endl;
			for (auto value : remainingArgs) {
				setting->deleteValue(value, configHandler);
			}
		}
	} else {
		out << "The key \"" << qPrintable(key) << "\" does not support this operation" << endl;
		return false;
	}

	return true;
}
