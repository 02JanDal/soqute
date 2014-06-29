#pragma once

#include "basecommand.h"

class ShowCommand : public BaseCommand
{
	Q_OBJECT
public:
	ShowCommand(ConfigurationHandler *configHandler, PackageList *packages,
				QObject *parent = 0);

protected:
	void setupParser() override;
	bool executeImplementation() override;
};
