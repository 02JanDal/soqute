#pragma once

#include <QMap>
#include <QStringList>
#include <QString>

namespace GetFiles
{

extern QString version;

QMap<QString, QStringList> binaryFiles();
QMap<QString, QStringList> docFiles();
QMap<QString, QStringList> exampleFiles();
QMap<QString, QStringList> importsFiles();
QMap<QString, QStringList> libExecFiles();
QMap<QString, QStringList> mkSpecFiles();
QMap<QString, QStringList> phrasebookFiles();
QMap<QString, QStringList> qmlFiles();
QMap<QString, QStringList> translationFiles();
QMap<QString, QStringList> includeFolders();
QMap<QString, QStringList> libFiles();
QMap<QString, QStringList> pluginNames();

QMap<QString, QString> descriptions();
QMap<QString, QStringList> dependencies();
QMap<QString, QMap<QString, QStringList> > nativeDependencies();

}
