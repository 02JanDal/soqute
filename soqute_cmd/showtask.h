#ifndef SHOWTASK_H
#define SHOWTASK_H

#include "task.h"

class ShowTask : public Task
{
    Q_OBJECT
public:
    explicit ShowTask(PackageList *packages, QObject *parent = 0);

    void setOutputNativePackagesOnly(const QString& manager);
	void setPackageId(const QString& id);
	void setPackageVersion(const QString& version);
    void setPackagePlatform(const QString& platform);

	virtual QString taskName() const { return "ShowTask"; }

protected:
    virtual void start();

private:
    QString m_pkgManager;
    QString m_packageId;
    QString m_packageVersion;
    QString m_packagePlatform;
};

#endif // SHOWTASK_H
