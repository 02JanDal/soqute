#ifndef SEARCHTASK_H
#define SEARCHTASK_H

#include "task.h"

#include <QtCore/QStringList>

class SearchTask : public Task
{
    Q_OBJECT
public:
    explicit SearchTask(PackageList *packages, QObject *parent = 0);

    void setQueries(const QStringList& queries);
    void setMatchDescription(const bool matchDescription);

	virtual QString taskName() const { return "SearchTask"; }

protected:
    virtual void start();

private:
    QStringList m_queries;
    bool m_matchDescription;
};

#endif // SEARCHTASK_H
