#include "configurationhandler.h"

#include <QSettings>
#include <QStandardPaths>

#include "util_core.h"
#include "installedpackages.h"

ConfigurationHandler* ConfigurationHandler::m_instance;

ConfigurationHandler::ConfigurationHandler(QObject *parent) :
	QObject(parent), m_installedPackages(0)
{
    QSettings::setDefaultFormat(QSettings::IniFormat);
    m_settings = new QSettings();

    setupDefaults();

    m_instance = this;
}

ConfigurationHandler::~ConfigurationHandler()
{
    delete m_settings;
}

ConfigurationHandler *ConfigurationHandler::instance()
{
    return m_instance;
}

void ConfigurationHandler::addRepositoryUrl(const QUrl &url)
{
    QList<QVariant> prevValue = m_settings->value("Repositories").toList();
    prevValue.append(url);

    // remove duplicates
    QList<QVariant> out;
	for (const QVariant& url : prevValue) {
        if (!out.contains(url)) {
            out.append(url);
        }
    }

    m_settings->setValue("Repositories", prevValue);
    m_settings->sync();
}
void ConfigurationHandler::removeRepositoryUrl(const QUrl &url)
{
    QList<QVariant> prevValue = m_settings->value("Repositories").toList();
    prevValue.removeAll(url);
    m_settings->setValue("Repositories", prevValue);
    m_settings->sync();
}
QList<QUrl> ConfigurationHandler::repositoryUrls() const
{
    QList<QVariant> variantList = m_settings->value("Repositories").toList();
    QList<QUrl> urlList;
	for (const QVariant& variant : variantList) {
        urlList.append(variant.toUrl());
    }
    return urlList;
}

void ConfigurationHandler::setPackageManager(const QString &mngr)
{
    m_settings->setValue("PackageManager", mngr);
    m_settings->sync();
}
QString ConfigurationHandler::packageManager() const
{
	return m_settings->value("PackageManager").toString();
}

void ConfigurationHandler::setInstallRoot(const QString &dir)
{
	m_settings->setValue("InstallRoot", dir);
	m_settings->sync();
}

QString ConfigurationHandler::installRoot() const
{
	return m_settings->value("InstallRoot").toString();
}

InstalledPackages *ConfigurationHandler::installedPackages()
{
	if (m_installedPackages == 0) {
		m_installedPackages = new InstalledPackages(m_settings, this);
	}
	return m_installedPackages;
}

void ConfigurationHandler::setupDefaults()
{
    if (!m_settings->contains("PackageManager")) {
        m_settings->setValue("PackageManager", Util::defaultPackageManager());
    }
	if (!m_settings->contains("InstallRoot")) {
		m_settings->setValue("InstallRoot",
					 #if defined(Q_OS_UNIX)
							 QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first() + "/Qt"
					 #elif defined(Q_OS_WIN32)
							 "C:\Qt"
					 #else
					 #error Your operating system is not supported
					 #endif
							 );
	}

    m_settings->sync();
}
