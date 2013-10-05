#ifndef CONFIRMTASK_H
#define CONFIRMTASK_H

#include <QThread>
#include "task.h"

QT_BEGIN_NAMESPACE
class QFile;
QT_END_NAMESPACE

class StdInThread;

class ConfirmTask : public Task
{
	Q_OBJECT
public:
	enum ConfirmationType
	{
		Install, Remove
	};

	explicit ConfirmTask(ConfirmationType type, PackageList *packages, QObject *parent = 0);
	~ConfirmTask();

	virtual QString taskName() const { return "ConfirmTask"; }

protected:
	virtual void start();

private slots:
	void receivedFromStandardInput(const QString& data);

private:
	ConfirmationType m_type;

	StdInThread* m_stdin;
};

class StdInThread : public QThread
{
	Q_OBJECT
	friend class ConfirmTask;
private:
	StdInThread(QObject* parent = 0);

signals:
	void lineReceived(const QString& data);

private:
	virtual void run();
	bool shouldRun;
};

#endif // CONFIRMTASK_H
