#pragma once

#include <QThread>

class Package;

class AbstractInstaller : public QThread
{
	Q_OBJECT
public:
	explicit AbstractInstaller(QObject *parent = 0);

	virtual void run() override;

public
slots:
	virtual void install(const Package *package, const QString &fileName) = 0;
	virtual void remove(const Package *package, const QString &fileName) = 0;

signals:
	void installPackageBegin(const Package *package);
	void installPackageEnd(const Package *package);
	void removePackageBegin(const Package *package);
	void removePackageEnd(const Package *package);
	void message(const QString &msg);
	void error(const QString &msg);
};
