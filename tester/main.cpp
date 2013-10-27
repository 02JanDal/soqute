#include <QCoreApplication>
#include <QProcess>
#include <QDir>
#include <QCryptographicHash>
#include <QTextStream>
#include <QSettings>
#include <QUrl>
#include <QDebug>

#include <cstdio>
#include <iostream>

QTextStream out(stdout);

class Output : public QObject
{
    Q_OBJECT
public:
    Output(QIODevice* device) : QObject(), m_device(device)
    {
        connect(m_device, SIGNAL(readyRead()), this, SLOT(dataRead()));
    }

private slots:
    void dataRead()
    {
        out << m_device->readAll() << flush;
    }

private:
    QIODevice* m_device;
};

QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

bool confirm() {
    char c;
    std::cin >> c;
    return c == 'y';
}

QDir appDir;
QByteArray runProgram(const QString& program, const QStringList& arguments = QStringList())
{
    out << "  Running " << program << " " << arguments.join(' ') << endl << flush;
    QProcess proc;
    proc.setProgram(program);
    proc.setWorkingDirectory(appDir.absolutePath());
    proc.setArguments(arguments);
    proc.setProcessChannelMode(QProcess::MergedChannels);
    proc.setProcessEnvironment(env);
    Output out(&proc);
    proc.start();
    proc.waitForStarted(-1);
    proc.waitForFinished(-1);
    return proc.readAll();
}

bool compareEntry(const QFileInfo& i1, const QFileInfo& i2);
bool compareFiles(const QString& f1, const QString& f2)
{
    // some preliminary checks
    QFileInfo i1(f1);
    QFileInfo i2(f2);
    if (i1.size() != i2.size() ||
            i1.permissions() != i2.permissions() ||
            i1.fileName() != i2.fileName() ||
            !i1.exists() || !i2.exists()) {
        return false;
    }

    // the real check
    QFile file1(f1);
    QFile file2(f2);
    if (!file1.open(QFile::ReadOnly) || !file2.open(QFile::ReadOnly)) {
        return false;
    } else {
        QCryptographicHash hash1(QCryptographicHash::Sha256);
        QCryptographicHash hash2(QCryptographicHash::Sha256);
        hash1.addData(&file1);
        hash2.addData(&file2);
        return hash1.result() == hash2.result();
    }
}
bool compareDirectories(const QDir& d1, const QDir& d2)
{
    if (d1.entryList() != d2.entryList()) {
        return false;
    } else {
        foreach(const QFileInfo & info, d1.entryInfoList()) {
            if (!compareEntry(info, QFileInfo(d2, info.fileName()))) {
                return false;
            }
        }
    }
    return true;
}
bool compareEntry(const QFileInfo& i1, const QFileInfo& i2)
{
    if (i1.isFile() && i2.isFile()) {
        return compareFiles(i1.absoluteFilePath(), i2.absoluteFilePath());
    } else if (i1.isDir() && i2.isFile()) {
        return compareDirectories(i1.absoluteFilePath(), i2.absoluteFilePath());
    } else {
        return false;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    appDir = a.applicationDirPath();
    appDir.cdUp();
    appDir.cd("soqute_cmd");

    out << "Preparing..." << endl << flush;

    QDir dir = QDir::home();
    if (dir.exists("soqute_test")) {
        dir.cd("soqute_test");
        dir.removeRecursively();
    }
    dir = QDir::home();
    dir.mkdir("soqute_test");
    dir.cd("soqute_test");

    out << "Welcome to the soqute tester. This program will use the following directory:" << endl << flush;
    out << "    " << dir.absolutePath() << endl << flush;
    out << "  Continue? " << flush;
    if (!confirm()) {
        return 0;
    }

    out << "Setting up enviroment variables..." << endl << flush;
    env.insert("LD_LIBRARY_PATH", QDir::cleanPath(QDir(a.applicationDirPath()).absoluteFilePath("../soqute_core")));
    QString PATH = env.value("PATH");
    PATH += ":" + QDir::cleanPath(QDir(a.applicationDirPath()).absoluteFilePath("../soqute_cmd"));
    env.insert("PATH", PATH);

    out << "Creating backup of configuration file..." << endl << flush;
    const QString configFile = QDir::home().absoluteFilePath(".config/JanDalheimer/soqute.ini");
    QFile::copy(configFile, configFile + ".backup");

    out << "Setting up soqute..." << endl << flush;
    const QString repoUrl = QUrl::fromLocalFile(a.applicationDirPath() + QDir::separator() + "test_meta.json").toString();
    runProgram("./soqute_cmd", QStringList() << "config" << "--add" << "repositories" << repoUrl);
    runProgram("./soqute_cmd", QStringList() << "config" << "--set" << "install-prefix" << dir.absolutePath());

    out << "Installing..." << endl << flush;
    runProgram("./soqute_cmd", QStringList() << "install" << "--silent" << "testpackage/1.0.0#generic-test-platform");

    QDir installDir(dir);
    installDir.cd("generic-test-platform");
    installDir.cd("1.0.0");
    if (!installDir.exists()) {
        out << "ERROR: Root install directory was not created" << endl << flush;
    } else {
        QDir fromDir(a.applicationDirPath());
        fromDir.cd("packages");
        fromDir.cd("full");
        if (!compareDirectories(installDir, fromDir)) {
            out << "ERROR: Directories don't match" << endl << flush;
        } else {
            out << "Removing first package..." << endl << flush;
            runProgram("./soqute_cmd", QStringList() << "remove" << "--silent" << "testpackage1");
            fromDir.cdUp();
            fromDir.cd("package1");
            if (!compareDirectories(installDir, fromDir)) {
                out << "ERROR: Directories don't match" << endl << flush;
            } else {
                out << "Removing second package..." << endl << flush;
                runProgram("./soqute_cmd", QStringList() << "remove" << "--silent" << "testpackage2");
                fromDir.cdUp();
                fromDir.cd("package2");
                if (!compareDirectories(installDir, fromDir)) {
                    out << "ERROR: Directories don't match" << endl << flush;
                } else {
                    if (installDir.count() > 0) {
                        out << "ERROR: Installation directory is not empty" << endl << flush;
                    } else {
                        out << "SUCCESS: Everything went fine" << endl << flush;
                    }
                }
            }
        }
    }

    out << "Restoring backup of configuration file..." << endl << flush;
    QDir::home().remove(configFile);
    QDir::home().rename(configFile + ".backup", configFile);

    out << "Enter any key + enter to continue cleaning up " << flush; confirm();

    out << "Cleaning up...";
    if (dir.exists()) {
        dir.removeRecursively();
    }

    return a.exec();
}

#include "main.moc"
