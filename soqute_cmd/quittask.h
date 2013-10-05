#ifndef QUITTASK_H
#define QUITTASK_H

#include "task.h"

class QuitTask : public Task
{
    Q_OBJECT
public:
    explicit QuitTask(PackageList *packages, QObject *parent = 0);

	virtual QString taskName() const { return "QuitTask"; }

protected:
    void start();
};

#endif // QUITTASK_H
