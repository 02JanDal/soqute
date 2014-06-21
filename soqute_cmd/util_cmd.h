#pragma once

class QCommandLineParser;

namespace Util
{
void handleCommonArguments(QCommandLineParser* parser);
bool confirm();
}
