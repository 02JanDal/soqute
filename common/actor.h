#pragma once

#include <QObject>

#include <QQueue>
#include <QVariant>

/**
 * \brief The Actor class provides a common base for classes that need to have error/status
 *        reporting, and that can finish.
 *
 * It provides functions/signals for both blocking and non-blocking usage
 */
class Actor : public QObject
{
	Q_OBJECT
public:
	Actor(QObject *parent = 0);

	typedef QPair<int, QVariant> Message;

	QString messageToString(const Message &msg) const
	{
		return messageToStringImpl(msg.first, msg.second);
	}
	bool hasMessage() const
	{
		return !m_messages.isEmpty();
	}
	Message takeLastMessage()
	{
		return m_messages.dequeue();
	}

	bool isDone() const
	{
		return m_isDone;
	}
	bool isSuccess() const
	{
		return m_isSuccess;
	}

private:
	QQueue<Message> m_messages;
	bool m_isDone;
	bool m_isSuccess;

protected:
	static const int messageTypeErrorOffset = 50;

	void reset()
	{
		m_isDone = false;
		m_isSuccess = true;
	}
	void setIsSuccess(const bool isSuccess)
	{
		m_isSuccess = isSuccess;
	}
	void addMessage(const int msg, const QVariant &data = QVariant());
	virtual QString messageToStringImpl(const int msg, const QVariant &data) const = 0;

protected
slots:
	void finish();

signals:
	void message(const int msg, const QVariant &data);
	void done(const bool success);
};
