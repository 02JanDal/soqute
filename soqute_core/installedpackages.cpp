#include "installedpackages.h"

#include <QSettings>
#include <QDebug>

#include "package.h"
#include "util_core.h"

InstalledPackages::InstalledPackages(QSettings *settings, QObject *parent)
	: QObject(parent), m_settings(settings)
{
}

bool InstalledPackages::isPackageInstalled(const QString &id, const QString &version, const Platform &host, const Platform &target) const
{
	// TODO version.isNull() should match all versions
	const QString key = generateKey(id, version, host.toString(), target.toString());
	return m_settings->allKeys().contains(key) && m_settings->value(key).toBool();
}

bool InstalledPackages::isPackageInstalled(PackagePointer package) const
{
	return isPackageInstalled(package->id(), package->version(), package->host(), package->target());
}

void InstalledPackages::setPackageInstalled(PackagePointer package)
{
	m_settings->setValue(generateKey(package), true);
	m_settings->sync();
}

void InstalledPackages::setPackageUninstalled(PackagePointer package)
{
	m_settings->setValue(generateKey(package), false);
	m_settings->sync();
}

QList<const Package *> InstalledPackages::installedPackages(PackageList *packages) const
{
	m_settings->beginGroup("InstalledPackages");

	QList<PackagePointer> out;

	for (const QString &id : m_settings->childGroups()) {
		m_settings->beginGroup(id);
		for (const QString &host : m_settings->childGroups()) {
			m_settings->beginGroup(host);
			for (const QString &target : m_settings->childGroups()) {
				m_settings->beginGroup(target);
				for (const QString &version : m_settings->childKeys()) {
					out.append(packages->package(id, version, Platform::fromString(host), Platform::fromString(target)));
				}
				m_settings->endGroup();
			}
			m_settings->endGroup();
		}
		m_settings->endGroup();
	}

	m_settings->endGroup();

	return out;
}

bool InstalledPackages::isNewestInstalled(PackagePointer package)
{
	m_settings->beginGroup("InstalledPackages");
	if (!m_settings->childGroups().contains(package->id())) {
		m_settings->endGroup();
		return false;
	}
	m_settings->beginGroup(package->id());
	if (!m_settings->childGroups().contains(package->host().toString())) {
		m_settings->endGroup();
		m_settings->endGroup();
		return false;
	}
	m_settings->beginGroup(package->host().toString());
	if (!m_settings->childGroups().contains(package->target().toString())) {
		m_settings->endGroup();
		m_settings->endGroup();
		m_settings->endGroup();
		return false;
	}
	m_settings->beginGroup(package->target().toString());
	QStringList installedVersions = m_settings->childKeys();
	for (const QString &installedVersion : installedVersions) {
		if (Util::isVersionHigherThan(installedVersion, package->version())) {
			m_settings->endGroup();
			m_settings->endGroup();
			m_settings->endGroup();
			m_settings->endGroup();
			return false;
		}
	}
	m_settings->endGroup();
	m_settings->endGroup();
	m_settings->endGroup();
	m_settings->endGroup();
	return true;
}

const QString InstalledPackages::generateKey(PackagePointer pkg) const
{
	return generateKey(pkg->id(), pkg->version(), pkg->host().toString(), pkg->target().toString());
}
const QString InstalledPackages::generateKey(const QString &id, const QString &version,
											 const QString &host, const QString &target) const
{
	return QString("InstalledPackages/%1/%2/%3/%4").arg(id, host, target, version);
}
