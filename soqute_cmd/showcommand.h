#ifndef SHOW_H
#define SHOW_H

#include "basecommand.h"

class ShowCommand : public BaseCommand
{
    Q_OBJECT
public:
	ShowCommand(ConfigurationHandler* configHandler, PackageList* packages, QObject* parent = 0);

protected:
    virtual void setupParser();
	virtual bool executeImplementation();
};

#endif // SHOW_H
