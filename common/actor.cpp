#include "actor.h"

Actor::Actor(QObject *parent)
	: QObject(parent), m_isDone(false), m_isSuccess(true)
{
}

void Actor::addMessage(const int msg, const QVariant &data)
{
	m_messages.enqueue(qMakePair(msg, data));
	emit message(msg, data);
	if (msg >= messageTypeErrorOffset) {
		setIsSuccess(false);
	}
}

void Actor::finish()
{
	m_isDone = true;
	emit done(m_isSuccess);
}
