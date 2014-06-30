#pragma once

#include "abstractinstaller.h"

#include <QStringList>

class QJSEngine;

class JSInstaller : public AbstractInstaller
{
	Q_OBJECT
public:
	explicit JSInstaller(QObject *parent = 0);

public
slots:
	virtual void install(const Package *package, const QString &fileName) override;
	virtual void remove(const Package *package, const QString &fileName) override;

private:
	friend class JSObject;
	void emitError(const QString &message);

private:
	QJSEngine *m_engine;
	QStringList m_errors;

	void setupEngine();
	void setupEngineForPackage(const Package *package);
};
