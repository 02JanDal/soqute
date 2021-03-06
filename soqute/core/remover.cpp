#include "remover.h"

#include <QDir>

#include "configurationhandler.h"
#include "installedpackages.h"
#include "jsinstaller.h"
#include "util_core.h"
#include "package.h"
#include "filesystem.h"

Remover::Remover(QObject *parent) : Actor(parent), m_remover(new JSInstaller(this))
{
	connect(m_remover, &AbstractInstaller::error, this, &Remover::errorRemoving);
	connect(m_remover, &AbstractInstaller::removePackageBegin, this,
			&Remover::removePackageBegin);
	connect(m_remover, &AbstractInstaller::removePackageEnd, this,
			&Remover::removePackageEnd);
	connect(m_remover, &AbstractInstaller::finished, this, &Remover::finish);
	connect(this, &Remover::removePackage, m_remover,
			&AbstractInstaller::remove, Qt::QueuedConnection);
}

QString Remover::messageToStringImpl(const int msg, const QVariant &data) const
{
	switch (msg) {
	case Removing: {
		PackagePointer pkg = data.value<PackagePointer>();
		return tr("Removing %1...").arg(Util::createFriendlyName(pkg));
	}
	case Removed: {
		PackagePointer pkg = data.value<PackagePointer>();
		return tr("Removed %1").arg(Util::createFriendlyName(pkg));
	}
	case RemoveError: {
		return tr("Removal error: %1").arg(data.toString());
	}
	case OtherError: {
		return tr("Error: %1").arg(data.toString());
	}
	default:
		return QString(data.toString());
	}
}

void Remover::remove(const QList<const Package *> &toRemove)
{
	// reset
	setIsSuccess(true);

	m_packagesToRemove = toRemove;
	removeNextPackage();
}

void Remover::removePackageBegin(const Package *package)
{
	addMessage(Removing, QVariant::fromValue(package));
}

void Remover::removePackageEnd(const Package *package)
{
	addMessage(Removed, QVariant::fromValue(package));
	ConfigurationHandler::instance()->installedPackages()->setPackageUninstalled(package);
	if (!m_packagesToRemove.isEmpty()) {
		removeNextPackage();
	} else {
		finalCleanup();
		m_remover->quit();
		m_remover->wait();
		finish();
	}
}

void Remover::errorRemoving(const QString &msg)
{
	addMessage(RemoveError, msg);
}

void Remover::finalCleanup()
{
	if (FS::exists(ConfigurationHandler::instance()->installRoot())) {
		try
		{
			FS::removeEmptyRecursive(QDir(ConfigurationHandler::instance()->installRoot()));
		}
		catch (Exception &e)
		{
			addMessage(OtherError, e.message());
		}
	}
}

void Remover::removeNextPackage()
{
	const Package *package = m_packagesToRemove.takeFirst();
	emit removePackage(package, Util::removalScriptsDirectory().absoluteFilePath(QString(
									"%1-%2-%3-%4-remove.js").arg(package->id(), package->version(),
															  package->host().toString(), package->target().toString())));
}
