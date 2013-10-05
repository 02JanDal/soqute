#include "taskhandler.h"

#include <QtCore/QThread>
#include <QtCore/QDebug>

#include "task.h"
#include "package.h"

TaskHandler::TaskHandler(PackageList *packages, QObject *parent) :
    QObject(parent), m_currentTask(0), m_packages(packages)
{
}

void TaskHandler::addTask(Task *task)
{
	m_tasks.enqueue(task);
}

void TaskHandler::setUsedPackages(const QList<const Package *> &usedPackages)
{
	m_usedPackages = usedPackages;
}

void TaskHandler::run()
{
    taskFinished(true);
}

void TaskHandler::taskFinished(bool success)
{
    if (!success) {
        emit finished(false);
        return;
    }

    if (m_currentTask) {
		m_usedPackages = m_currentTask->usedPackages();
        disconnect(m_currentTask);
        m_currentTask->deleteLater();
    }

    if (m_tasks.isEmpty()) {
        emit finished(true);
        return;
	}

    m_currentTask = m_tasks.dequeue();
	m_currentTask->setUsedPackages(m_usedPackages);
	qDebug() << "   starting task" << m_currentTask->taskName();
    connect(m_currentTask, SIGNAL(finished(bool)), this, SLOT(taskFinished(bool)));
    m_currentTask->execute();
}
