#pragma once

#include <QString>
#include <exception>

class Exception : public std::exception
{
public:
	Exception(const QString &message) : std::exception(), m_message(message)
	{
	}
	~Exception() noexcept
	{
	}
	const char *what() const noexcept
	{
		return m_message.toLatin1().constData();
	}
	QString message() const
	{
		return m_message;
	}

private:
	QString m_message;
};

#define DECLARE_EXCEPTION(name)                                                                \
	class name##Exception : public ::Exception                                                 \
	{                                                                                          \
	public:                                                                                    \
		name##Exception(const QString &message) : Exception(message)                           \
		{                                                                                      \
		}                                                                                      \
	}
