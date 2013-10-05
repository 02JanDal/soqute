#include "util_cmd.h"

#include <QRegularExpression>

#include <qcommandlineparser.h>
#include <cstdio>
#include <iostream>

#include "textstream.h"

namespace Util
{

void handleCommonArguments(QCommandLineParser *parser)
{
    if (parser->isSet(QStringLiteral("version"))) {
        printf("%s %s\n", qPrintable(QCoreApplication::applicationName()), qPrintable(QCoreApplication::applicationVersion()));
        ::exit(0);
    }
    if (!parser->applicationDescription().isEmpty() && parser->isSet(QStringLiteral("help"))) {
        parser->showHelp();
        ::exit(0);
    }
    if (parser->unknownOptionNames().count() == 1) {
        fprintf(stderr, "Unknown option '%s'.\n", qPrintable(parser->unknownOptionNames().first()));
        ::exit(1);
    }
    if (parser->unknownOptionNames().count() > 1) {
        fprintf(stderr, "Unknown options: %s.\n", qPrintable(parser->unknownOptionNames().join(QLatin1String(", "))));
        ::exit(1);
    }
}

bool confirm()
{
    out << "Continue? [y/n] " << flush;

    const unsigned char size = 16;
    char buf[size];
    std::cin.getline(buf, size);
    const QString answer = QString::fromLocal8Bit(buf, size);
    if (!answer.toLower().startsWith("y")) {
        return false;
    }
    return true;
}

}
