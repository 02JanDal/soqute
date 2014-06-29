#pragma once

#include <QObject>

class QSettings;

class PackageList;
class Package;
struct Platform;
typedef const Package *PackagePointer;

class InstalledPackages : public QObject
{
	Q_OBJECT
public:
	explicit InstalledPackages(QSettings *settings, QObject *parent = 0);

	bool isPackageInstalled(const QString &id, const QString &version, const Platform &host, const Platform &target) const;
	bool isPackageInstalled(PackagePointer package) const;
	void setPackageInstalled(PackagePointer package);
	void setPackageUninstalled(PackagePointer package);
	QList<const Package *> installedPackages(PackageList *packages) const;
	bool isNewestInstalled(PackagePointer package);

signals:

public
slots:

private:
	QSettings *m_settings;

	const QString generateKey(PackagePointer pkg) const;
	const QString generateKey(const QString &id, const QString &version,
							  const QString &host, const QString &target) const;
};
