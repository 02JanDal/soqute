#ifndef REMOVER_H
#define REMOVER_H

#include <QObject>
#include <QQueue>
#include <QVariant>

class Package;
class AbstractInstaller;

class Remover : public QObject
{
	Q_OBJECT
public:
	explicit Remover(QObject *parent = 0);

	enum Message
	{
		Removing,
		Removed,
		RemoveError
	};
	static QString messageToString(const Message msg, const QVariant &data);
	bool hasMessage() const { return !m_messages.isEmpty(); }
	QPair<Message, QVariant> takeLastMessage() { return m_messages.dequeue(); }

	bool isDone() const { return m_isDone; }
	bool isSuccess() const { return m_isSuccess; }
	
signals:

public slots:
	void remove(const QList<const Package*>& toRemove);
	
private slots:
	void removePackageBegin(const Package* package);
	void removePackageEnd(const Package* package);
	void errorRemoving(const QString& msg);

    void finalCleanup();

	void finish(bool success = true);
	
private:
	AbstractInstaller* m_remover;
	QList<const Package*> m_packagesToRemove;
	QQueue<QPair<Message, QVariant> > m_messages;
	bool m_isDone;
	bool m_isSuccess;

	void removeNextPackage();

	void addMessage(const Message msg, const QVariant& data = QVariant());

signals:
	void removePackage(const Package* package, const QString& fileName);
	void message(const Message msg, const QVariant& data);
	void done(const bool success);
};

#endif // REMOVER_H
