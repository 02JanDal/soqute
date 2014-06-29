#pragma once

#include "installcommandbase.h"

class BuildsystemCommand : public InstallCommandBase
{
	Q_OBJECT
public:
	BuildsystemCommand(ConfigurationHandler *configHandler, PackageList *packages,
					   QObject *parent = 0);

protected:
	void setupParser() override;
	bool preMetadataLoadImplementation() override;
	bool executeImplementation() override;
};
