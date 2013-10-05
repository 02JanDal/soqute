#ifndef TASKHANDLER_H
#define TASKHANDLER_H

#include <QtCore/QObject>
#include <QtCore/QQueue>

#include "soqute_core_global.h"

class Task;
class PackageList;
class Package;

/**
 * The TaskHandler class takes care of starting, finishing and deleting tasks
 */
class SOQUTE_CORESHARED_EXPORT TaskHandler : public QObject
{
    Q_OBJECT
public:
    explicit TaskHandler(PackageList* packages, QObject *parent = 0);

    /**
     * addTask adds a task to the end of the list. TaskHandler will take ownership of the task and delete it when it is finished
     * @param task The task to enqueue
     */
    void addTask(Task* task);

    PackageList* packages() const { return m_packages; }

	QList<const Package*> usedPackages() { return m_usedPackages; }
	void setUsedPackages(const QList<const Package*>& usedPackages);

public slots:
    /**
     * Starts the processing of tasks. This function is non-blocking
     */
    void run();
    
signals:
    /**
     * Emitted when all tasks have finished processing, or if a task didn't finish successfully
     */
    void finished(bool success);
    
private slots:
    void taskFinished(bool success);
    
private:
    QQueue<Task*> m_tasks;

    /// The currently executed task
    Task* m_currentTask;

    PackageList* m_packages;

    /// A list of all packages that are used in the current operation
	QList<const Package*> m_usedPackages;
};

#endif // TASKHANDLER_H
