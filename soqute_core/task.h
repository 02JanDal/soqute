#ifndef TASK_H
#define TASK_H

#include <QtCore/QObject>

#include "soqute_core_global.h"

class PackageList;
class Package;

/**
 * The Task class is a base class for all tasks
 * A tasks can be pretty much anything that needs to be done, fetching files, unpacking, installing, removing etc.
 */
class SOQUTE_CORESHARED_EXPORT Task : public QObject
{
    Q_OBJECT
public:
    explicit Task(PackageList *packages, QObject *parent = 0);

	void setUsedPackages(QList<const Package *> packages);
	QList<const Package *> usedPackages() const { return m_usedPackages; }

	virtual QString taskName() const = 0;

protected:
    /**
     * start is called when the task is started. This function should be non-blocking.
     * If threading is needed it has to be setup inside this function.
     */
    virtual void start() = 0;

    void delayedFinish();

    PackageList* m_packages;
	QList<const Package*> m_usedPackages;
    
signals:
    /**
     * This signal should be emitted when the task has done his job. He will soon after be deleted, so no more processing should take place after this signal has been emitted.
     */
    void finished(bool success = true);

	void usedPackagesChanged(const QList<const Package*> packages);
    
public slots:
    /**
     * This, together with \see finished, is the main access function to the task. Call this function to start the task.
     * \internal Internally this calls start
     */
    void execute();
};

#endif // TASK_H
