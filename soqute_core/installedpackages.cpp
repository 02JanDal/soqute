#include "installedpackages.h"

#include <QSettings>
#include <QDebug>

#include "package.h"
#include "util_core.h"

InstalledPackages::InstalledPackages(QSettings *settings, QObject *parent) :
	QObject(parent), m_settings(settings)
{
}

bool InstalledPackages::isPackageInstalled(const QString &id, const QString &version, const QString &platform) const
{
    const QString key = generateKey(id, version, platform);
    return m_settings->allKeys().contains(key) && m_settings->value(key).toBool();
}

void InstalledPackages::setPackageInstalled(const QString &id, const QString &version, const QString &platform)
{
    m_settings->setValue(generateKey(id, version, platform), true);
	m_settings->sync();
}

void InstalledPackages::setPackageUninstalled(const QString &id, const QString &version, const QString &platform)
{
    m_settings->setValue(generateKey(id, version, platform), false);
	m_settings->sync();
}

QList<const Package *> InstalledPackages::installedPackages(PackageList *packages) const
{
	m_settings->beginGroup("InstalledPackages");

	QList<const Package*> out;

	foreach (const QString& platform, m_settings->childGroups()) {
		m_settings->beginGroup(platform);
		foreach (const QString& id, m_settings->childGroups()) {
			m_settings->beginGroup(id);
			foreach (const QString& version, m_settings->childKeys()) {
				out.append(packages->package(id, version, platform));
			}
			m_settings->endGroup();
		}
		m_settings->endGroup();
	}

	m_settings->endGroup();

    return out;
}

bool InstalledPackages::isNewestInstalled(const QString &id, const QString &version, const QString &platform)
{
    m_settings->beginGroup("InstalledPackages");
    if (!m_settings->childGroups().contains(id)) {
        m_settings->endGroup();
        return false;
    }
    m_settings->beginGroup(id);
    if (!m_settings->childGroups().contains(platform)) {
        m_settings->endGroup();
        m_settings->endGroup();
        return false;
    }
    m_settings->beginGroup(platform);
    QStringList installedVersions = m_settings->childKeys();
    foreach (const QString& installedVersion, installedVersions) {
        if (Util::isVersionHigherThan(installedVersion, version)) {
            m_settings->endGroup();
            m_settings->endGroup();
            m_settings->endGroup();
            return false;
        }
    }
    m_settings->endGroup();
    m_settings->endGroup();
    m_settings->endGroup();
    return true;
}

const QString InstalledPackages::generateKey(const QString &id, const QString &version, const QString &platform) const
{
    return QString("InstalledPackages/%1/%2/%3").arg(id, platform, version);
}
