#ifndef SEARCHCOMMAND_H
#define SEARCHCOMMAND_H

#include "basecommand.h"

class SearchCommand : public BaseCommand
{
    Q_OBJECT
public:
	explicit SearchCommand(ConfigurationHandler* configHandler, PackageList* packages, QObject *parent = 0);

protected:
    virtual void setupParser() override;
	virtual bool executeImplementation() override;
};

#endif // SEARCHCOMMAND_H
