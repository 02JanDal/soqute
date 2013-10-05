#ifndef CONFIGURATIONHANDLER_H
#define CONFIGURATIONHANDLER_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

#include "soqute_core_global.h"

class QSettings;

class InstalledPackages;

class SOQUTE_CORESHARED_EXPORT ConfigurationHandler : public QObject
{
    Q_OBJECT
public:
    explicit ConfigurationHandler(QObject *parent = 0);
    ~ConfigurationHandler();

    static ConfigurationHandler* instance();

    void addRepositoryUrl(const QUrl& url);
    void removeRepositoryUrl(const QUrl& url);
    QList<QUrl> repositoryUrls() const;

    void setPackageManager(const QString& mngr);
    QString packageManager() const;

	void setInstallRoot(const QString& dir);
	QString installRoot() const;

	InstalledPackages* installedPackages();

private:
    static ConfigurationHandler* m_instance;

	InstalledPackages* m_installedPackages;

    QSettings* m_settings;

    void setupDefaults();
};

#endif // CONFIGURATIONHANDLER_H
