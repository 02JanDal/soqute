#include "quittask.h"

#include <QtCore/QCoreApplication>

QuitTask::QuitTask(PackageList *packages, QObject *parent) :
    Task(packages, parent)
{
}

void QuitTask::start()
{
    QCoreApplication::instance()->quit();
}
