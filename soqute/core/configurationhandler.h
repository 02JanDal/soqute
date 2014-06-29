#pragma once

#include <QObject>
#include <QUrl>
#include <QVariant>

#include "soqute_core_global.h"

class QSettings;

class InstalledPackages;

class SOQUTE_CORESHARED_EXPORT ConfigurationHandler : public QObject
{
	Q_OBJECT
public:
	explicit ConfigurationHandler(QObject *parent = 0);
	~ConfigurationHandler();

	static ConfigurationHandler *instance();

	void addRepositoryUrl(const QUrl &url);
	void removeRepositoryUrl(const QUrl &url);
	QList<QUrl> repositoryUrls() const;

	void setPackageManager(const QString &mngr);
	QString packageManager() const;

	void setInstallRoot(const QString &dir);
	QString installRoot() const;

	void setAllowBuildSystemInstall(const bool allow);
	bool allowBuildSystemInstall() const;

	void setTemporaryExtraRepository(const QUrl &repo);
	void setTemporaryInstallRoot(const QString &dir);

	InstalledPackages *installedPackages();

	void set(const QString &key, const QVariant &value);
	QVariant get(const QString &key, const QVariant &defaultValue = QVariant()) const;

private:
	static ConfigurationHandler *m_instance;

	InstalledPackages *m_installedPackages;

	QSettings *m_settings;

	QUrl m_temporaryExtraRepository;
	QString m_temporaryInstallRoot;

	void setupDefaults();
};
