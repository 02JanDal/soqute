#pragma once

#include "installcommandbase.h"

class InstallCommand : public InstallCommandBase
{
	Q_OBJECT
public:
	explicit InstallCommand(ConfigurationHandler *configHandler, PackageList *packages,
							QObject *parent = 0);

protected:
	void setupParser() override;
	bool executeImplementation() override;
};
