#pragma once

#include "basecommand.h"

class Package;
typedef const Package * PackagePointer;

class RemoveCommand : public BaseCommand
{
	Q_OBJECT
public:
	explicit RemoveCommand(ConfigurationHandler *configHandler, PackageList *packages,
						   QObject *parent = 0);

protected:
	virtual void setupParser() override;
	virtual bool executeImplementation() override;

private:
	bool isNewestInstalled(PackagePointer entity);
};
