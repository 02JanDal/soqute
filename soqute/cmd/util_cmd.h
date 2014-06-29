#pragma once

class QCommandLineParser;
class QString;
class QStringList;

namespace Util
{
void handleCommonArguments(QCommandLineParser *parser);
bool confirm();
QString niceJoin(const QStringList &list);
}
