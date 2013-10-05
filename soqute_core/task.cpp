#include "task.h"

#include <QtCore/QTimer>
#include <QDebug>

Task::Task(PackageList *packages, QObject *parent) :
    QObject(parent), m_packages(packages)
{
}

void Task::setUsedPackages(QList<const Package *> packages)
{
	m_usedPackages = packages;
	emit usedPackagesChanged(m_usedPackages);
}

void Task::delayedFinish()
{
    QTimer::singleShot(0, this, SIGNAL(finished(bool)));
}

void Task::execute()
{
    start();
}
