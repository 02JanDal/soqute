#pragma once

#include "basecommand.h"

class ConfigCommand : public BaseCommand
{
    Q_OBJECT
public:
	ConfigCommand(ConfigurationHandler* configHandler, PackageList* packages, QObject* parent = 0);

    bool needMetadata() const override { return false; }

protected:
    virtual void setupParser() override;
	virtual bool executeImplementation() override;
};
