#ifndef INSTALLEDPACKAGES_H
#define INSTALLEDPACKAGES_H

#include <QObject>

class QSettings;

class PackageList;
class Package;

class InstalledPackages : public QObject
{
	Q_OBJECT
public:
	explicit InstalledPackages(QSettings* settings, QObject *parent = 0);

	bool isPackageInstalled(const QString& id, const QString& version, const QString& platform) const;
	void setPackageInstalled(const QString& id, const QString& version, const QString& platform);
	void setPackageUninstalled(const QString& id, const QString& version, const QString& platform);
	QList<const Package*> installedPackages(PackageList* packages) const;
    bool isNewestInstalled(const QString& id, const QString& version, const QString& platform);
	
signals:
	
public slots:
	
private:
	QSettings* m_settings;

	const QString generateKey(const QString& id, const QString& version, const QString& platform) const;
};

#endif // INSTALLEDPACKAGES_H
