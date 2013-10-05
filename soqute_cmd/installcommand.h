#ifndef INSTALLCOMMAND_H
#define INSTALLCOMMAND_H

#include "basecommand.h"

class InstallCommand : public BaseCommand
{
    Q_OBJECT
public:
	explicit InstallCommand(ConfigurationHandler* configHandler, PackageList* packages, QObject *parent = 0);

protected:
    virtual void setupParser();
	virtual bool executeImplementation();
};

#endif // INSTALLCOMMAND_H
