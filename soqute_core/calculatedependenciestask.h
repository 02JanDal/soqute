#ifndef CALCULATEDEPENDENCIESTASK_H
#define CALCULATEDEPENDENCIESTASK_H

#include "task.h"
#include "soqute_core_global.h"

QT_BEGIN_NAMESPACE
template <typename T>
class QFutureWatcher;
template <>
class QFutureWatcher<void>;
QT_END_NAMESPACE

class DependencyCalculator;

class SOQUTE_CORESHARED_EXPORT CalculateDependenciesTask : public Task
{
    Q_OBJECT
public:
    explicit CalculateDependenciesTask(PackageList *packages, QObject *parent = 0);
    ~CalculateDependenciesTask();

	virtual QString taskName() const { return "CalculateDependenciesTask"; }

protected:
    void start();

private slots:
	void calculationFinished();

signals:
	void noSuchPackage(const QString& id, const QString& version, const QString& platform);

private:
    DependencyCalculator* m_calculator;
	QFutureWatcher<void>* m_watcher;
};

#endif // CALCULATEDEPENDENCIESTASK_H
