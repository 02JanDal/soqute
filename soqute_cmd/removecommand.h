#pragma once

#include "basecommand.h"

class Package;

class RemoveCommand : public BaseCommand
{
	Q_OBJECT
public:
	explicit RemoveCommand(ConfigurationHandler* configHandler, PackageList* packages, QObject *parent = 0);

protected:
	virtual void setupParser() override;
	virtual bool executeImplementation() override;

private:
    bool isNewestInstalled(const Package *entity);
};
