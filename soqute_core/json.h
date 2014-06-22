#pragma once

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QUrl>

#include "exception.h"

namespace Json
{
DECLARE_EXCEPTION(Json);

enum Requirement {
	Required
};

void write(const QJsonDocument &doc, const QString &filename);
void write(const QJsonObject &object, const QString &filename);
void write(const QJsonArray &array, const QString &filename);

QJsonDocument ensureDocument(const QByteArray &data);
QJsonDocument ensureDocument(const QString &filename);
QJsonObject ensureObject(const QJsonDocument &doc, const QString &what = "Document");
QJsonArray ensureArray(const QJsonDocument &doc, const QString &what = "Document");
QJsonObject ensureObject(const QJsonValue &value, const QString &what = "Value");
QJsonObject ensureObject(const QJsonObject &parent, const QString &key,
						 const QString &what = "Value");
QJsonArray ensureArray(const QJsonValue &value, const QString &what = "Value");
QJsonArray ensureArray(const QJsonObject &parent, const QString &key,
					   const QString &what = "Value");

QJsonValue toJson(const QUrl &url);
QJsonValue toJson(const QByteArray &data);
QJsonValue toJson(const QDateTime &datetime);

// template magic!

template <typename T>
typename std::enable_if<std::is_same<T, QString>::value, T>::type
	ensureIsType(const QJsonValue &value, const Requirement requirement = Required,
				 const QString &what = "Value")
{
	if (!value.isString()) {
		throw JsonException(what + " is not a string");
	}
	return value.toString();
}

template <typename T>
typename std::enable_if<std::is_same<T, bool>::value, T>::type
	ensureIsType(const QJsonValue &value, const Requirement requirement = Required,
				 const QString &what = "Value")
{
	if (!value.isBool()) {
		throw JsonException(what + " is not a bool");
	}
	return value.toBool();
}

template <typename T>
typename std::enable_if<std::is_same<T, double>::value, T>::type
	ensureIsType(const QJsonValue &value, const Requirement requirement = Required,
				 const QString &what = "Value")
{
	if (!value.isDouble()) {
		throw JsonException(what + " is not a double");
	}
	return value.toDouble();
}

template <typename T>
typename std::enable_if<std::is_same<T, int>::value, T>::type
	ensureIsType(const QJsonValue &value, const Requirement requirement = Required,
				 const QString &what = "Value")
{
	double doubl = ensureIsType<double>(value, requirement, what);
	if (fmod(doubl, 1) != 0) {
		throw JsonException(what + " is not an integer");
	}
	return doubl;
}

template <typename T>
typename std::enable_if<std::is_same<T, QDateTime>::value, T>::type
	ensureIsType(const QJsonValue &value, const Requirement requirement = Required,
				 const QString &what = "Value")
{
	const QString string = ensureIsType<QString>(value, requirement, what);
	const QDateTime datetime = QDateTime::fromString(string, Qt::ISODate);
	if (!datetime.isValid()) {
		throw JsonException(what + " is not a ISO formatted date/time value");
	}
	return datetime;
}

template <typename T>
typename std::enable_if<std::is_same<T, QUrl>::value, T>::type
	ensureIsType(const QJsonValue &value, const Requirement requirement = Required,
				 const QString &what = "Value")
{
	const QString string = ensureIsType<QString>(value, requirement, what);
	const QUrl url = QUrl(string, QUrl::StrictMode);
	if (!url.isValid()) {
		throw JsonException(what + " is not a correctly formatted URL");
	}
	return url;
}

template <typename T>
typename std::enable_if<std::is_same<T, QByteArray>::value, T>::type
	ensureIsType(const QJsonValue &value, const Requirement requirement = Required,
				 const QString &what = "Value")
{
	const QString string = ensureIsType<QString>(value, requirement, what);
	// ensure that the string can be safely cast to Latin1
	if (string != QString::fromLatin1(string.toLatin1())) {
		throw JsonException(what + " is not encodable as Latin1");
	}
	return QByteArray::fromHex(string.toLatin1());
}

template <typename T>
T ensureIsType(const QJsonValue &value, const T default_, const QString &what = "Value")
{
	if (value.isUndefined()) {
		return default_;
	}
	return ensureIsType<T>(value, Required, what);
}
template <typename T>
T ensureIsType(const QJsonObject &parent, const QString &key,
			   const Requirement requirement = Required,
			   const QString &what = "__placeholder__")
{
	const QString localWhat = QString(what).replace("__placeholder__", '\'' + key + '\'');
	if (!parent.contains(key)) {
		throw JsonException(localWhat + "s parent does not contain " + localWhat);
	}
	return ensureIsType<T>(parent.value(key), requirement, localWhat);
}
template <typename T>
T ensureIsType(const QJsonObject &parent, const QString &key, const T default_,
			   const QString &what = "__placeholder__")
{
	const QString localWhat = QString(what).replace("__placeholder__", '\'' + key + '\'');
	if (!parent.contains(key)) {
		return default_;
	}
	return ensureIsType<T>(parent.value(key), default_, localWhat);
}
}
