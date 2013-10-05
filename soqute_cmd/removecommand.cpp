#include "removecommand.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>
#include <QDir>

#include <iostream>

#include <qcommandlineparser.h>

#include "package.h"
#include "remover.h"
#include "textstream.h"
#include "util_cmd.h"
#include "util_core.h"
#include "configurationhandler.h"
#include "installedpackages.h"
#include "dependencycalculator.h"

// FIXME dependency checking (removing qtgui = removing qtgui AND qtwidgets)

RemoveCommand::RemoveCommand(ConfigurationHandler* configHandler, PackageList* packages, QObject *parent) :
    BaseCommand(configHandler, packages, parent)
{
}

void RemoveCommand::setupParser()
{
    parser = new QCommandLineParser();
    parser->addVersionOption();
    parser->addHelpOption(tr("Removes packages"));
    parser->addOption(QCommandLineOption(QStringList() << "no-platform-behavior",
                                         tr("Describes how I should behave if you don't give me a platform. Possible options: host (use the host platform, the default), abort"),
                                         tr("behavior"), "host"));
    parser->addOption(QCommandLineOption(QStringList() << "no-version-behavior",
                                         tr("Describes how I should behave if you don't give me a version. Possible options: clean (removes all except the newest), all (removes all), abort (the default)"),
                                         tr("behavior"), "abort"));
    parser->addOption(QCommandLineOption(QStringList() << "silent", tr("Won't ask you for questions")));
    parser->setRemainingArgumentsHelpText(tr("<package-name>[/<version>][#<platform>]*"));
}

bool RemoveCommand::executeImplementation()
{
    if (parser->remainingArguments().isEmpty()) {
        out << "You need to specify at least one query\n" << flush;
        return false;
    }

    PackagePointerList pkgs;
    // argument parsing
    {
        // captures expressions like this: <name>[/<version>][#<platform>]
        // examples: qtbase/5.0.0#win32-g++
        //           qtjsbackend
        //           qtpim#linux-g++-32
        //           qtquick1/5.1.0
        QRegularExpression exp("([a-z0-9\\-_\\+]*)(/[a-z0-9\\-\\+\\.]*)?(#[a-z0-9\\-\\+]*)?");

        foreach (PackagePointer entity, packages->entities()) {
            foreach (const QString& argument, parser->remainingArguments()) {
                QRegularExpressionMatch match = exp.match(argument);
                const QString id = match.captured(1);
                const QString version = match.captured(2).remove(0, 1);
                const QString platform = match.captured(3).remove(0, 1);

                if (parser->value("no-platform-behavior") == "abort" && platform.isEmpty()) {
                    out << "You need to specify a platform for the following argument: " << argument << "\n" << flush;
                    return false;
                }

                if (parser->value("no-version-behavior") == "abort" && version.isEmpty()) {
                    out << "You need to specify a version for the following argument: " << argument << ", or check out --no-version-behavior\n" << flush;
                    return false;
                }

                if (entity->id() == id) {
                    if ((parser->value("no-platform-behavior") == "host" && entity->platform() == Util::currentPlatform()) ||
                            (entity->platform() == platform)) {
                        // clean, all
                        if (parser->value("no-version-behavior") == "all" ||
                                (parser->value("no-version-behavior") == "clean" && isNewestInstalled(entity)) ||
                                entity->version() == version) {
                            if (ConfigurationHandler::instance()->installedPackages()->isPackageInstalled(entity->id(), entity->version(), entity->platform())) {
                                pkgs.append(entity);
                            }
                        }
                    }
                }
            }
        }
    }

    // dependency calculation
    {
        DependencyCalculator depCalculator(packages, pkgs, this);
        if (!depCalculator.calculateDependencies(DependencyCalculator::Reverse)) {
            return false;
        }
        pkgs = depCalculator.result();
    }

    if (pkgs.isEmpty()) {
        out << "Nothing to do\n" << flush;
        return true;
    }

    // confirmation
    {
        if (!parser->isSet("silent")) {
            out << "You have choosen to remove the following packages:\n"
                << "  " << flush;
            QStringList packages;
            foreach (PackagePointer package, pkgs) {
                packages.append(QString("%1 v%2 (%3)").arg(package->id(), package->version(), package->platform()));
            }
            out << packages.join(", ") << '\n' << flush;

            if (!Util::confirm()) {
                return false;
            }
        }
    }

    // remove
    {
        Remover remover(this);
        remover.remove(pkgs);

        while (!remover.isDone()) {
            qApp->processEvents();
            qApp->sendPostedEvents();
            while (remover.hasMessage()) {
                QPair<Remover::Message, QVariant> msg = remover.takeLastMessage();
                out << remover.messageToString(msg.first, msg.second) << "\n" << flush;
            }
        }
        while (remover.hasMessage()) {
            QPair<Remover::Message, QVariant> msg = remover.takeLastMessage();
            out << remover.messageToString(msg.first, msg.second) << "\n" << flush;
        }
    }

    return true;
}

bool RemoveCommand::isNewestInstalled(const Package *entity)
{
    return ConfigurationHandler::instance()->installedPackages()->isNewestInstalled(entity->id(), entity->version(), entity->platform());
}
