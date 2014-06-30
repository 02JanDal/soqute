#include "util_cmd.h"

#include <QRegularExpression>
#include <QCommandLineParser>

#include <iostream>

#include "textstream.h"

namespace Util
{

void handleCommonArguments(QCommandLineParser *parser)
{
	if (parser->optionNames().contains("version") && parser->isSet(QStringLiteral("version"))) {
		out << QCoreApplication::applicationName() << " "
			<< QCoreApplication::applicationVersion() << endl;
		::exit(0);
	}
	if (!parser->applicationDescription().isEmpty() && parser->isSet(QStringLiteral("help"))) {
		out << parser->helpText();
		::exit(0);
	}
	if (parser->unknownOptionNames().count() == 1) {
		out << "Unknown option: '" << parser->unknownOptionNames().first() << "'." << endl;
		::exit(1);
	}
	if (parser->unknownOptionNames().count() > 1) {
		out << "Unknown options: '" << parser->unknownOptionNames().join(", ") << "'." << endl;
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
	if (!answer.toLower().startsWith('y')) {
		return false;
	}
	return true;
}

QString niceJoin(const QStringList &list)
{
	QString out = list.join(", ");
	if (list.size() >= 2) {
		out.replace(out.lastIndexOf(", "), 2, " and ");
	}
	return out;
}

}
