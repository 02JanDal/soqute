#pragma once

#include "abstractinstaller.h"

class QJSEngine;

class JSInstaller : public AbstractInstaller
{
	Q_OBJECT
public:
	explicit JSInstaller(QObject *parent = 0);

public slots:
	virtual void install(const Package *package, const QString &fileName, QString *errorString = 0) override;
	virtual void remove(const Package *package, const QString &fileName, QString *errorString = 0) override;

private:
	QJSEngine* m_engine;

	void setupEngine();
	void setupEngineForPackage(const Package* package);
};
