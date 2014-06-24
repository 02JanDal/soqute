#include <QCoreApplication>
#include <QProcess>
#include <QDir>
#include <QCryptographicHash>
#include <QTextStream>
#include <QSettings>
#include <QUrl>
#include <QDebug>

#include <iostream>

QTextStream out(stdout);

class Output : public QObject
{
	Q_OBJECT
public:
	Output(QIODevice *device) : QObject(), m_device(device)
	{
		connect(m_device, SIGNAL(readyRead()), this, SLOT(dataRead()));
	}

private
slots:
	void dataRead()
	{
		while (m_device->canReadLine()) {
			out << "    " << m_device->readLine();
		}
		out << flush;
	}

private:
	QIODevice *m_device;
};

QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

bool confirm()
{
	char c;
	std::cin >> c;
	return c == 'y';
}

QDir appDir;
bool runProgram(const QString &program, const QStringList &arguments = QStringList())
{
	out << "  Running " << program << " " << arguments.join(' ') << endl << flush;
	QProcess proc;
	proc.setProgram(program);
	proc.setWorkingDirectory(appDir.absolutePath());
	proc.setArguments(arguments);
	proc.setProcessEnvironment(env);
	proc.setProcessChannelMode(QProcess::MergedChannels);
	Output output(&proc);
	proc.start();
	proc.waitForStarted(-1);
	proc.waitForFinished(-1);
	if (proc.exitCode() != QProcess::NormalExit || proc.error() != QProcess::UnknownError) {
		return false;
	}
	return true;
}

bool compareEntry(const QFileInfo &i1, const QFileInfo &i2);
bool compareFiles(const QString &f1, const QString &f2)
{
	// some preliminary checks
	QFileInfo i1(f1);
	QFileInfo i2(f2);
	if (i1.size() != i2.size() || i1.permissions() != i2.permissions() ||
		i1.fileName() != i2.fileName() || !i1.exists() || !i2.exists()) {
		out << "files " << f1 << " and" << f2 << " differ" << endl << flush;
		return false;
	}

	// the real check
	QFile file1(f1);
	QFile file2(f2);
	if (!file1.open(QFile::ReadOnly) || !file2.open(QFile::ReadOnly)) {
		out << "couldn't open a file" << endl << flush;
		return false;
	} else {
		QCryptographicHash hash1(QCryptographicHash::Sha256);
		QCryptographicHash hash2(QCryptographicHash::Sha256);
		hash1.addData(&file1);
		hash2.addData(&file2);
		if (hash1.result() == hash2.result()) {
			return true;
		} else {
			out << "files " << file1.fileName() << " and " << file2.fileName() << " differ"
				<< endl << flush;
			return false;
		}
	}
}
bool compareDirectories(const QDir &d1, const QDir &d2)
{
	d1.refresh();
	d2.refresh();
	if (!d1.exists()) {
		out << d1.absolutePath() << " doesn't exist" << endl << flush;
	} else if (!d2.exists()) {
		out << d2.absolutePath() << " doesn't exist" << endl << flush;
	} else if (d1.entryList() != d2.entryList()) {
		out << "contents of " << d1.absolutePath() << " are different from those of "
			<< d2.absolutePath() << endl << flush;
		out << d1.absolutePath() << ": "
			<< d1.entryList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs).join(", ") << "\n"
			<< d2.absolutePath() << ": "
			<< d2.entryList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs).join(", ") << endl
			<< flush;
		return false;
	} else {
		for (const QFileInfo &info : d1.entryInfoList(QDir::NoDotAndDotDot)) {
			if (!compareEntry(info, QFileInfo(d2, info.fileName()))) {
				return false;
			}
		}
	}
	return true;
}
bool compareEntry(const QFileInfo &i1, const QFileInfo &i2)
{
	if (i1.isFile() && i2.isFile()) {
		return compareFiles(i1.absoluteFilePath(), i2.absoluteFilePath());
	} else if (i1.isDir() && i2.isDir()) {
		return compareDirectories(i1.absoluteFilePath(), i2.absoluteFilePath());
	} else {
		return false;
	}
}

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	appDir = a.applicationDirPath();
	appDir.refresh();

	out << "Preparing..." << endl << flush;

	QDir dir = QDir::home();
	if (dir.exists("soqute_test")) {
		dir.cd("soqute_test");
		dir.removeRecursively();
	}
	dir = QDir::home();
	dir.mkdir("soqute_test");
	dir.cd("soqute_test");
	dir.refresh();

	out << "Welcome to the soqute tester. This program will use the following directory:"
		<< endl << flush;
	out << "    " << dir.absolutePath() << endl << flush;
	out << "  Continue? " << flush;
	if (!a.arguments().contains("no-interaction") && !confirm()) {
		return 0;
	}

	out << "Setting up enviroment variables..." << endl << flush;
	env.insert("LD_LIBRARY_PATH", QDir::cleanPath(a.applicationDirPath()));
	QString PATH = env.value("PATH");
	PATH +=
		":" + QDir::cleanPath(a.applicationDirPath());
	env.insert("PATH", PATH);

	out << "Creating backup of configuration file..." << endl << flush;
	const QString configFile = QDir::home().absoluteFilePath(".config/JanDalheimer/soqute.ini");
	QFile::copy(configFile, configFile + ".backup");

	out << "Setting up soqute..." << endl << flush;
	const QString repoUrl = QUrl::fromLocalFile(a.applicationDirPath() + QDir::separator() +
												"test_meta.json").toString();
	if (runProgram("./soqute_cmd", QStringList() << "config"
												 << "--add"
												 << "repositories" << repoUrl)) {
		if (runProgram("./soqute_cmd", QStringList() << "config"
													 << "--set"
													 << "install-prefix"
													 << dir.absolutePath())) {

			out << "Installing..." << endl << flush;
			if (runProgram("./soqute_cmd", QStringList()
											   << "install"
											   << "--silent"
											   << "testpackage/1.0.0#generic-test-platform")) {

				QDir installDir(dir);
				installDir.cd("generic-test-platform");
				installDir.cd("1.0.0");
				installDir.refresh();
				if (!installDir.exists()) {
					out << "ERROR: Root install directory was not created" << endl << flush;
				} else {
					QDir fromDir(a.applicationDirPath());
					fromDir.cd("packages");
					fromDir.cd("full");
					fromDir.refresh();
					if (!compareDirectories(installDir, fromDir)) {
						out << "ERROR: Directories don't match" << endl << flush;
					} else {
						out << "SUCCESS: Installing packages" << endl << flush;
						out << "Removing first package..." << endl << flush;
						if (runProgram("./soqute_cmd",
									   QStringList()
										   << "remove"
										   << "--silent"
										   << "testpackage1/1.0.0#generic-test-platform")) {
							fromDir.cdUp();
							fromDir.cd("package2");
							fromDir.refresh();
							if (!compareDirectories(installDir, fromDir)) {
								out << "ERROR: Directories don't match" << endl << flush;
							} else {
								out << "Removing second package..." << endl << flush;
								if (runProgram(
										"./soqute_cmd",
										QStringList()
											<< "remove"
											<< "--silent"
											<< "testpackage2/1.0.0#generic-test-platform")) {
									installDir.refresh();
									const auto entries = installDir.entryList(QDir::NoDotAndDotDot);
									if (entries.size() > 0) {
										out << "ERROR: Installation directory is not empty (entries left: " << entries.join(", ") << ")"
											<< endl << flush;
									} else {
										out << "SUCCESS: Removing packages" << endl << flush;
										out << "SUCCESS: Everything went fine" << endl << flush;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	out << "Restoring backup of configuration file..." << endl << flush;
	QDir::home().remove(configFile);
	QDir::home().rename(configFile + ".backup", configFile);

	out << "Cleaning up..." << endl << flush;
	if (dir.exists()) {
		dir.removeRecursively();
	}

	return 0;
}

#include "main.moc"
