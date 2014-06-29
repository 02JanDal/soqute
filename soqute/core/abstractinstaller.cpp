#include "abstractinstaller.h"

AbstractInstaller::AbstractInstaller(QObject *parent) : QThread(parent)
{
}

void AbstractInstaller::run()
{
	exec();
}
