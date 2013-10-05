#ifndef REMOVECOMMAND_H
#define REMOVECOMMAND_H

#include "basecommand.h"

class Package;

class RemoveCommand : public BaseCommand
{
	Q_OBJECT
public:
	explicit RemoveCommand(ConfigurationHandler* configHandler, PackageList* packages, QObject *parent = 0);

protected:
	virtual void setupParser();
	virtual bool executeImplementation();

private:
    bool isNewestInstalled(const Package *entity);
};

#endif // REMOVECOMMAND_H
