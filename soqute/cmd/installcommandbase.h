#pragma once

#include "basecommand.h"

#include "package.h"
#include "downloader.h"

class QAuthenticator;

class CommandLineAuthenticator : public AbstractAuthenticator
{
public:
	bool authenticate(QNetworkReply *reply, QAuthenticator *authenticator) override;
};
/// authenticator that fails if it's invoked
class FailAuthenticator : public AbstractAuthenticator
{
public:
	bool authenticate(QNetworkReply *, QAuthenticator *) override;
};

class InstallCommandBase : public BaseCommand
{
	Q_OBJECT
public:
	InstallCommandBase(ConfigurationHandler *configHandler, PackageList *packages,
					   QObject *parent = 0);

protected:
	void baseChecks();
	void constructPackageList(const QStringList cliPackages);
	void installPackages(AbstractAuthenticator *authenticator);

	PackagePointerList m_pkgs;
};
