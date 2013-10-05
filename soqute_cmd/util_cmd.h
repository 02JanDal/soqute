#ifndef UTILCMD_H
#define UTILCMD_H

class QCommandLineParser;

namespace Util
{
void handleCommonArguments(QCommandLineParser* parser);
bool confirm();
}

#endif // UTILCMD_H
