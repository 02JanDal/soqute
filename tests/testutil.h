#pragma once

#include <QTextStream>
#include <QObject>
#include <QIODevice>
#include <QProcessEnvironment>
#include <functional>

extern QTextStream out;

class QFileInfo;
class QDir;

namespace TestUtil
{
extern QProcessEnvironment env;

class Output : public QObject
{
	Q_OBJECT
public:
	Output(QIODevice *device);

private
slots:
	void dataRead();

private:
	QIODevice *m_device;
};

bool runProgram(const QString &program, const QStringList &arguments = QStringList());

bool confirm();

bool compareFiles(const QString &f1, const QString &f2);
bool compareDirectories(const QDir &d1, const QDir &d2);
bool compareEntry(const QFileInfo &i1, const QFileInfo &i2);

/// A very simple implementation of a ScopeGuard. Give it a lambda, and it will be run on scope
/// exit.
class ScopeGuard
{
	typedef std::function<void()> Lambda;
	Lambda m_lambda;

public:
	ScopeGuard(const Lambda &lambda) : m_lambda(lambda)
	{
	}
	~ScopeGuard() noexcept
	{
		m_lambda();
	}
};
}
