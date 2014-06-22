#pragma once

#include "actor.h"

class Package;
class AbstractInstaller;

class Remover : public Actor
{
	Q_OBJECT
public:
	explicit Remover(QObject *parent = 0);

	enum MessageType {
		Removing,
		Removed,
		RemoveError = messageTypeErrorOffset,
		OtherError
	};

public
slots:
	void remove(const QList<const Package *> &toRemove);

private
slots:
	void removePackageBegin(const Package *package);
	void removePackageEnd(const Package *package);
	void errorRemoving(const QString &msg);

	void finalCleanup();

	QString messageToStringImpl(const int msg, const QVariant &data) const override;

private:
	AbstractInstaller *m_remover;
	QList<const Package *> m_packagesToRemove;

	void removeNextPackage();

signals:
	void removePackage(const Package *package, const QString &fileName);
};
