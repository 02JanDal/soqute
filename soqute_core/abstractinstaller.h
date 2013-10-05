#ifndef ABSTRACTINSTALLER_H
#define ABSTRACTINSTALLER_H

#include <QThread>

class Package;

class AbstractInstaller : public QThread
{
	Q_OBJECT
public:
	explicit AbstractInstaller(QObject *parent = 0);

	virtual void run();

public slots:
	virtual void install(const Package* package, const QString& fileName, QString* errorString = 0) = 0;
	virtual void remove(const Package* package, const QString& fileName, QString* errorString = 0) = 0;

signals:
	void installPackageBegin(const Package* package);
	void installPackageEnd(const Package* package);
	void removePackageBegin(const Package* package);
	void removePackageEnd(const Package* package);
	void message(const QString& msg);
	void error(const QString& msg);
};

#endif // ABSTRACTINSTALLER_H
