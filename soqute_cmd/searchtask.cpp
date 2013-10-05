#include "searchtask.h"

#include <QtCore/QTextStream>
#include <QtCore/QDebug>

#include "package.h"
#include "configurationhandler.h"
#include "installedpackages.h"

SearchTask::SearchTask(PackageList *packages, QObject *parent) :
    Task(packages, parent), m_matchDescription(false)
{
}

void SearchTask::setQueries(const QStringList &queries)
{
    m_queries = queries;
}

void SearchTask::setMatchDescription(const bool matchDescription)
{
    m_matchDescription = matchDescription;
}

QString padded(const QString& string, const int size, const QChar character = ' ')
{
    if (string.size() >= size) {
        return string;
    }
    QString padding;
    padding.fill(character, size - string.size());
    return string + padding;
}

void SearchTask::start()
{
    QTextStream str(stdout);
    const QList<const Package*> packages = m_packages->findPackages(m_queries, m_matchDescription);

    if (packages.isEmpty()) {
        str << "No packages where found\n";
        delayedFinish();
        return;
    }

    int longestId = 0;
    int longestPlatform = 0;

    foreach (const Package* package, packages) {
        longestId = qMax(longestId, package->id().length());
        longestPlatform = qMax(longestPlatform, package->platform().length());
    }

    // we use a map here to sort the results
    QMultiMap<QString, QString> rows;
	rows.insert("", QString("\E[1m I  ") + padded("ID", longestId) + "  " + padded("Platform", longestPlatform) + "  Description\E[0m");
    foreach (const Package* package, packages) {
		QString str = QString("  ") + padded(package->id(), longestId) + "  " + padded(package->platform(), longestPlatform) + "  " + package->description().split('\n')[0];
		if (ConfigurationHandler::instance()->installedPackages()->isPackageInstalled(package->id(), package->version(), package->platform())) {
			str = " i" + str;
		} else {
			str = "  " + str;
		}
        rows.insert(package->platform(), str);
    }

    foreach (const QString& row, rows) {
        str << row << "\n";
    }

    delayedFinish();
}
