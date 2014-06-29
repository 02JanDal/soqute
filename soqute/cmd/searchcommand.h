#pragma once

#include "basecommand.h"

class SearchCommand : public BaseCommand
{
	Q_OBJECT
public:
	explicit SearchCommand(ConfigurationHandler *configHandler, PackageList *packages,
						   QObject *parent = 0);

protected:
	void setupParser() override;
	bool executeImplementation() override;
};
