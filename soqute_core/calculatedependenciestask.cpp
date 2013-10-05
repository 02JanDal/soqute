#include "calculatedependenciestask.h"

#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QDebug>
#include <iostream>

#include "dependencycalculator.h"
#include "package.h"
#include "util_core.h"

CalculateDependenciesTask::CalculateDependenciesTask(PackageList *packages, QObject *parent) :
    Task(packages, parent)
{
	m_watcher = new QFutureWatcher<void>(this);
	connect(m_watcher, SIGNAL(finished()), this, SLOT(calculationFinished()));
}

CalculateDependenciesTask::~CalculateDependenciesTask()
{
    delete m_calculator;
}

void CalculateDependenciesTask::start()
{
	std::cout << "Calculating dependencies..." << std::endl;
	m_calculator = new DependencyCalculator(m_packages, m_usedPackages);
	connect(this, SIGNAL(usedPackagesChanged(QList<const Package*>)), m_calculator, SLOT(setStartingPoint(QList<const Package*>)));
	connect(m_calculator, SIGNAL(noSuchPackage(QString,QString,QString)), this, SIGNAL(noSuchPackage(QString,QString,QString)));

	QFuture<void> future = QtConcurrent::run(m_calculator, &DependencyCalculator::calculateDependencies, (const Package**)0);
	m_watcher->setFuture(future);
}

void CalculateDependenciesTask::calculationFinished()
{
	m_usedPackages = Util::cleanPackagePointerList(m_calculator->result());

    emit finished(true);
}
