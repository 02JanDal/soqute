#ifndef CONFIG_H
#define CONFIG_H

#include "basecommand.h"

class ConfigCommand : public BaseCommand
{
    Q_OBJECT
public:
	ConfigCommand(ConfigurationHandler* configHandler, PackageList* packages, QObject* parent = 0);

    bool needMetadata() const { return false; }

protected:
    virtual void setupParser();
	virtual bool executeImplementation();
};

#endif // CONFIG_H
