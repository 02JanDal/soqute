#ifndef FINDINITIALPACKAGESTASK_H
#define FINDINITIALPACKAGESTASK_H

#include "task.h"

#include <QStringList>

class FindInitialPackagesTask : public Task
{
	Q_OBJECT
public:
	explicit FindInitialPackagesTask(PackageList* packages, QObject *parent = 0);

	virtual QString taskName() const { return "FindInitialPackagesTask"; }

	void setArguments(const QStringList& arguments) { m_arguments = arguments; }

signals:
	void invalidPackageArgument(const QString& argument);

protected:
	virtual void start();

private:
	QStringList m_arguments;
};

#endif // FINDINITIALPACKAGESTASK_H
