#pragma once

#include <QObject>

class QCommandLineParser;

class ConfigurationHandler;
class PackageList;

class BaseCommand : public QObject
{
    Q_OBJECT
public:
	explicit BaseCommand(ConfigurationHandler* configHandler, PackageList* packages, QObject *parent = 0);

    void showHelp();
	bool execute(const QStringList& args);

    virtual bool needMetadata() const { return true; }

protected: // variables
    QCommandLineParser* parser;
	ConfigurationHandler* configHandler;
	PackageList* packages;

protected: // methods
    virtual void setupParser() = 0;
	virtual bool executeImplementation() = 0;
};
