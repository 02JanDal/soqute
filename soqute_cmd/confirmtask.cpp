#include "confirmtask.h"

#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <cstdio>
#include <iostream>

#include "package.h"

ConfirmTask::ConfirmTask(ConfirmationType type, PackageList *packages, QObject *parent) :
	Task(packages, parent), m_type(type)
{
	m_stdin = new StdInThread(this);
	connect(m_stdin, SIGNAL(lineReceived(QString)), this, SLOT(receivedFromStandardInput(QString)));
}

ConfirmTask::~ConfirmTask()
{
	m_stdin->wait();
	delete m_stdin;
}

void ConfirmTask::start()
{
	QTextStream out(stdout);
	out << "You have choosen to install the following packages:\n"
		<< "  ";
	QStringList packages;
	foreach (PackagePointer package, m_usedPackages) {
		packages.append(QString("%1 v%2 (%3)").arg(package->id(), package->version(), package->platform()));
	}
	out << packages.join(", ") << '\n'
		<< "Continue? [y/n] ";

	m_stdin->start();
}

void ConfirmTask::receivedFromStandardInput(const QString &data)
{
	if (!data.startsWith('y')) {
		m_usedPackages.clear();
	}
	emit finished(true);
}

StdInThread::StdInThread(QObject *parent) :
	QThread(parent)
{
}

void StdInThread::run()
{
	const unsigned char size = 16;
	char buf[size];
	std::cin.getline(buf, size);
	emit lineReceived(QString::fromLocal8Bit(buf, size));
}
