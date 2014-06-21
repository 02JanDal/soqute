#include <QCoreApplication>
#include <QTextStream>
#include <QStringList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <iostream>

#include <karchive.h>
#include <ktar.h>

#include "filegetters.h"

QTextStream out(stdout);
bool silent = true;

void setupDirectory(const QDir& dir) {
    if (!dir.mkdir("bin")) abort();
    if (!dir.mkdir("doc")) abort();
    if (!dir.mkdir("examples")) abort();
    if (!dir.mkdir("imports")) abort();
    if (!dir.mkdir("include")) abort();
    if (!dir.mkdir("lib")) abort();
    if (!dir.mkdir("lib/cmake")) abort();
    if (!dir.mkdir("lib/pkgconfig")) abort();
    if (!dir.mkdir("mkspecs")) abort();
    if (!dir.mkdir("phrasebooks")) abort();
    if (!dir.mkdir("plugins")) abort();
    if (!dir.mkdir("qml")) abort();
    if (!dir.mkdir("translations")) abort();
}

void copyFiles(const QStringList& files, const QDir& fromDir, const QDir& toDir, const QString& intendent = "    ")
{
    if (intendent.size() > 40) {
        out << "Infinite loop detected\n";
        out.flush();
        abort();
    }
	for (const QString& file : files) {
        if (!silent) {
            out << intendent << "Copying " << fromDir.absoluteFilePath(file) << " to " << toDir.absoluteFilePath(file) << "...\n";
        }
        if (file.contains('*')) {
            copyFiles(fromDir.entryList(QStringList() << file, QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs), fromDir, toDir, intendent);
        } else {
            if (QFileInfo(fromDir, file).isDir()) {
                QDir dir(toDir);
                if (!dir.exists(file)) {
                    if (!dir.mkpath(file)) {
                        abort();
                    }
                }
                dir.cd(file);
                QDir fDir(fromDir);
                fDir.cd(file);
                copyFiles(fDir.entryList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs), fDir, dir, intendent + "  ");
            }
            QFile::copy(fromDir.absoluteFilePath(file), toDir.absoluteFilePath(file));
        }
    }
}

typedef QMap<QString, QStringList>(*FileGetterFunc)();
struct FileGroup
{
    FileGroup(const QString& name, FileGetterFunc fileGetter) : name(name), fileGetter(fileGetter) {}
    QString name;
    FileGetterFunc fileGetter;
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList args = a.arguments();
    if (args.size() < 4) {
        out << "Usage: " << args[0] << " <version> <qtdir> <platform> [load]\n" << flush;
        return 1;
    }
    const QString version = args[1];
    const QDir qtdir = QDir(args[2]);
    const QString platform = args[3];
    if (args.size() > 5) {
        if (args[4] == "load") {
            silent = false;
        }
    }
    if (!QRegularExpression("\\d\\.\\d\\.\\d").match(version).hasMatch()) {
        out << "<version> has to be in the format #.#.#\n" << flush;
        return 1;
    }

    const QString short_version = version.left(3);

    GetFiles::version = version;

    // mappings etc.
    QList<FileGroup> mappings;
    mappings.append(FileGroup("bin", &GetFiles::binaryFiles));
    mappings.append(FileGroup("doc", &GetFiles::docFiles));
    mappings.append(FileGroup("examples", &GetFiles::exampleFiles));
    mappings.append(FileGroup("imports", &GetFiles::importsFiles));
    mappings.append(FileGroup("include", &GetFiles::includeFolders));
    mappings.append(FileGroup("lib", &GetFiles::libFiles));
    mappings.append(FileGroup("libexec", &GetFiles::libExecFiles));
    mappings.append(FileGroup("mkspecs", &GetFiles::mkSpecFiles));
    mappings.append(FileGroup("phrasebooks", &GetFiles::phrasebookFiles));
    mappings.append(FileGroup("plugins", &GetFiles::pluginNames));
    mappings.append(FileGroup("qml", &GetFiles::qmlFiles));
    mappings.append(FileGroup("translations", &GetFiles::translationFiles));
    QStringList modules;
	for (const FileGroup& files : mappings) {
        modules.append((files.fileGetter)().keys());
    }
    modules.removeDuplicates();

    out << "Copying files for the following modules: " << modules.join(", ") << "\n" << flush;

    QDir baseDir;
	for (const QString& module : modules) {
        if (baseDir.exists(module + ".tar.gz")) {
            out << "Skiping already existing module " << module << "\n" << flush;
            continue;
        }
        out << "Copying files for module " << module << "\n" << flush;
        baseDir.mkdir(module);
        QDir dir(baseDir);
        dir.cd(module);

		for (const FileGroup& files : mappings) {
            QStringList fs = (files.fileGetter)()[module];
            if (fs.isEmpty()) {
                continue;
            }
            out << "  Copying " << files.name << " files...\n" << flush;
            QDir fromDir(qtdir);
            fromDir.cd(files.name);
            QDir toDir(dir);
            toDir.mkdir(files.name);
            toDir.cd(files.name);
            copyFiles(fs, fromDir, toDir);
        }

        out << "  Creating install file for module " << module << "...\n" << flush;
        {
            QFile installFile(dir.absoluteFilePath("install.js"));
            if (!installFile.open(QFile::WriteOnly | QFile::Truncate)) {
                out << "Error opening install file: " << installFile.errorString() << "\n";
                return -1;
            }

            QTextStream js(&installFile);

            js << "FileSystem.mkpath(Paths.installPath);\n";
			for (const FileGroup& groups : mappings) {
                if (groups.fileGetter().contains(module)) {
                    js << "FileSystem.install(\"" << groups.name << "\", Paths.installPath + \"/" << groups.name << "\");\n";
                }
            }
            js << "\n";

            js.flush();
            installFile.close();
        }

        out << "  Creating removal file for module " << module << "...\n" << flush;
        {
            QFile removalFile(dir.absoluteFilePath("remove.js"));
            if (!removalFile.open(QFile::WriteOnly | QFile::Truncate)) {
                out << "Error opening remove file: " << removalFile.errorString() << "\n";
                return -1;
            }

            QTextStream js(&removalFile);

			for (const FileGroup& group : mappings) {
                QStringList files = (group.fileGetter)()[module];
				for (const QString& file : files) {
                    js << "FileSystem.remove(Paths.installPath + \"/" << group.name << "/" << file << "\");\n";
                }
            }
            js << "\n";

            js.flush();
            removalFile.close();
        }

        out << "  Creating archive for module " << module << "...\n" << flush;
        {
            KArchive* arch = new KTar(baseDir.absoluteFilePath(QString("%1-%2-%3.tar.gz").arg(module, version, platform)));
            arch->open(QIODevice::ReadWrite);
			for (const QString& directory : dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs)) {
                arch->addLocalDirectory(dir.absoluteFilePath(directory), directory);
            }
			for (const QString& file : dir.entryList(QDir::NoDotAndDotDot | QDir::Files)) {
                arch->addLocalFile(dir.absoluteFilePath(file), file);
            }
            arch->close();
        }
    }

    out << "Creating metadata file...\n" << flush;
    {
        QFile meta(baseDir.absoluteFilePath("meta.json"));
        meta.open(QFile::WriteOnly | QFile::Truncate);
        QMap<QString, QString> descs = GetFiles::descriptions();
        QMap<QString, QStringList> deps = GetFiles::dependencies();
        QMap<QString, QMap<QString, QStringList> > ndeps = GetFiles::nativeDependencies();
        QJsonArray root;
		for (const QString& module : modules) {
            QStringList dependencies = deps[module];
            dependencies.removeAll("");
            dependencies.removeDuplicates();
            QMap<QString, QStringList> nDependencies = ndeps[module];
            QJsonObject m;
            m.insert("id", module);
            m.insert("description", descs[module]);
            m.insert("version", version);
            m.insert("platform", platform);
            m.insert("url", QString("http://localhost/soqute/archives/%1-%2-%3.tar.gz").arg(module, version, platform));
            QJsonArray deps;
			for (const QString& dep : dependencies) {
                QJsonObject obj;
                obj.insert("id", dep);
                obj.insert("version", version);
                deps.append(obj);
            }
            m.insert("dependencies", deps);
            QJsonObject nativeDependencies;
			for (const QString& manager : nDependencies.keys()) {
                QJsonArray packages = QJsonArray::fromStringList(nDependencies[manager]);
                nativeDependencies.insert(manager, packages);
            }
            m.insert("nativeDependencies", nativeDependencies);
            root.append(m);
        }
        meta.write(QJsonDocument(root).toJson());
    }

    out << "Installing files to server...\n" << flush;
    {
        QDir serverRoot("/var/www");
        if (!serverRoot.exists("soqute")) {
            serverRoot.mkdir("soqute");
        }
        serverRoot.cd("soqute");
        if (!serverRoot.exists("archives")) {
            serverRoot.mkdir("archives");
        }
        serverRoot.cd("archives");
		for (const QString& archive : baseDir.entryList(QStringList() << "*.tar.gz", QDir::Files)) {
            out << "  Installing " << archive << " to server...\n" << flush;
            if (serverRoot.exists(archive)) {
                serverRoot.remove(archive);
            }
            QFile::copy(baseDir.absoluteFilePath(archive), serverRoot.absoluteFilePath(archive));
        }
        serverRoot.cdUp();
        out << "  Installing meta.json to server...\n" << flush;
        if (serverRoot.exists("meta.json")) {
            QFile server(serverRoot.absoluteFilePath("meta.json"));
            Q_ASSERT(server.open(QFile::ReadOnly | QFile::Truncate));
            QFile local(baseDir.absoluteFilePath("meta.json"));
            Q_ASSERT(local.open(QFile::ReadOnly));
            QJsonDocument serverDoc = QJsonDocument::fromJson(server.readAll());
            server.close();
            QJsonArray serverArray = serverDoc.array();
            QJsonDocument localDoc = QJsonDocument::fromJson(local.readAll());
            local.close();
            QJsonArray localArray = localDoc.array();
            QVariantList res = serverArray.toVariantList();
            res.append(localArray.toVariantList());
            Q_ASSERT(server.open(QFile::WriteOnly | QFile::Truncate));
            server.write(QJsonDocument(QJsonArray::fromVariantList(res)).toJson());
            server.close();
        } else {
            QFile::copy(baseDir.absoluteFilePath("meta.json"), serverRoot.absoluteFilePath("meta.json"));
        }
    }

    out << "Done\n" << flush;
    return 0;
}

#ifndef Q_OS_LINUX
#error This program will only work on linux based systems
#endif
