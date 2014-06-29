#include "json.h"

#include <QFile>
#include <QSaveFile>

#include "filesystem.h"

void Json::write(const QJsonDocument &doc, const QString &filename)
{
	FS::write(filename, doc.toJson());
}
void Json::write(const QJsonObject &object, const QString &filename)
{
	write(QJsonDocument(object), filename);
}
void Json::write(const QJsonArray &array, const QString &filename)
{
	write(QJsonDocument(array), filename);
}

QJsonDocument Json::ensureDocument(const QByteArray &data)
{
	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(data, &error);
	if (error.error != QJsonParseError::NoError) {
		throw JsonException("Error parsing JSON: " + error.errorString());
	}
	return doc;
}
QJsonDocument Json::ensureDocument(const QString &filename)
{
	return Json::ensureDocument(FS::read(filename));
}
QJsonObject Json::ensureObject(const QJsonDocument &doc, const QString &what)
{
	if (!doc.isObject()) {
		throw JsonException(what + " is not an object");
	}
	return doc.object();
}
QJsonArray Json::ensureArray(const QJsonDocument &doc, const QString &what)
{
	if (!doc.isArray()) {
		throw JsonException(what + " is not an array");
	}
	return doc.array();
}

QJsonObject Json::ensureObject(const QJsonValue &value, const QString &what)
{
	if (!value.isObject()) {
		throw JsonException(what + " is not an object");
	}
	return value.toObject();
}
QJsonObject Json::ensureObject(const QJsonObject &parent, const QString &key,
							   const QString &what)
{
	if (!parent.contains(key)) {
		throw JsonException(what + "s parent does not contain " + what);
	}
	return Json::ensureObject(parent.value(key), what);
}

QJsonArray Json::ensureArray(const QJsonValue &value, const QString &what)
{
	if (!value.isArray()) {
		throw JsonException(what + " is not an array");
	}
	return value.toArray();
}
QJsonArray Json::ensureArray(const QJsonObject &parent, const QString &key, const QString &what)
{
	if (!parent.contains(key)) {
		throw JsonException(what + "s parent does not contain " + what);
	}
	return Json::ensureArray(parent.value(key), what);
}

QJsonValue Json::toJson(const QUrl &url)
{
	return QJsonValue(url.toString(QUrl::FullyEncoded));
}
QJsonValue Json::toJson(const QByteArray &data)
{
	return QJsonValue(QString::fromLatin1(data.toHex()));
}
QJsonValue Json::toJson(const QDateTime &datetime)
{
	return QJsonValue(datetime.toString(Qt::ISODate));
}
