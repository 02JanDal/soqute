#pragma once

#include "basecommand.h"

class LoadMetadata;

class UpdateCommand : public BaseCommand
{
	Q_OBJECT
public:
	explicit UpdateCommand(LoadMetadata *metadataLoader, ConfigurationHandler *configHandler, PackageList *packages,
							QObject *parent = 0);

	bool needMetadata() const override { return false; } // we'll fetch it ourselves

protected:
	void setupParser() override;
	bool executeImplementation() override;

private:
	LoadMetadata *m_metadataLoader;
};
