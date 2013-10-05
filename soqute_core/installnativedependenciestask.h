#ifndef INSTALLNATIVEDEPENDENCIESTASK_H
#define INSTALLNATIVEDEPENDENCIESTASK_H

#include "task.h"

class InstallNativeDependenciesTask : public Task
{
	Q_OBJECT
public:
	explicit InstallNativeDependenciesTask(PackageList *packages, QObject *parent = 0);

	virtual QString taskName() const { return "InstallNativeDependenciesTask"; }

protected:
	virtual void start();
};

#endif // INSTALLNATIVEDEPENDENCIESTASK_H
