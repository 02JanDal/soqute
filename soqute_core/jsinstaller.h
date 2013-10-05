#ifndef QTSCRIPTINSTALLER_H
#define QTSCRIPTINSTALLER_H

#include "abstractinstaller.h"

class QJSEngine;

class JSInstaller : public AbstractInstaller
{
	Q_OBJECT
public:
	explicit JSInstaller(QObject *parent = 0);

public slots:
	virtual void install(const Package *package, const QString &fileName, QString *errorString = 0);
	virtual void remove(const Package *package, const QString &fileName, QString *errorString = 0);

private:
	QJSEngine* m_engine;

	void setupEngine();
	void setupEngineForPackage(const Package* package);
};

#endif // QTSCRIPTINSTALLER_H
