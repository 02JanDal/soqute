#pragma once

#include <QObject>

class QCommandLineParser;

class ConfigurationHandler;
class PackageList;

class BaseCommand : public QObject
{
	Q_OBJECT
public:
	explicit BaseCommand(ConfigurationHandler *configHandler, PackageList *packages,
						 QObject *parent = 0);

	void showHelp();
	bool preMetadataLoad(const QStringList &args);
	int execute();

	virtual bool needMetadata() const
	{
		return true;
	}

private:
	int m_explicitExitCode = 1;

protected: // variables
	QCommandLineParser *parser;
	ConfigurationHandler *configHandler;
	PackageList *packages;

protected: // methods
	virtual void setupParser() = 0;
	virtual bool preMetadataLoadImplementation() { return true; }
	virtual bool executeImplementation() = 0;

	void setExplicitExitCode(const int code) { m_explicitExitCode = code; }
};
