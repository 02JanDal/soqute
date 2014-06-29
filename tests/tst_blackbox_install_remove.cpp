#include <QTest>
#include <QUrl>
#include <QDir>

#include "testutil.h"

class BlackBox_InstallRemove : public QObject
{
	Q_OBJECT

	QDir dir;
	QString configFile;
	QDir installDir;
	QDir fromDir;
private
slots:
	void initTestCase()
	{
		out << "Preparing..." << endl << flush;

		dir = QDir::home();
		if (dir.exists("soqute_test")) {
			dir.cd("soqute_test");
			dir.removeRecursively();
		}
		dir = QDir::home();
		dir.mkdir("soqute_test");
		dir.cd("soqute_test");
		dir.refresh();

		out << "Setting up enviroment variables..." << endl << flush;
		{
			TestUtil::env.insert("LD_LIBRARY_PATH",
								 QDir::cleanPath(qApp->applicationDirPath()));
			QString PATH = TestUtil::env.value("PATH");
			PATH += ":" + QDir::cleanPath(qApp->applicationDirPath());
			TestUtil::env.insert("PATH", PATH);
		}

		out << "Creating backup of configuration file..." << endl << flush;
		configFile = QDir::home().absoluteFilePath(".config/JanDalheimer/soqute.ini");
		QFile::copy(configFile, configFile + ".backup");

		installDir = dir.absolutePath() + "/--/generic-test-platform/1.0.0";
		fromDir = qApp->applicationDirPath();
	}

	void setup_addRepositories()
	{
		const QString repoUrl = QUrl::fromLocalFile(
			qApp->applicationDirPath() + QDir::separator() + "test_meta.json").toString();
		QVERIFY(TestUtil::runProgram("./soqute_cmd", QStringList() << "config"
																   << "--add"
																   << "repositories"
																   << repoUrl));
	}
	void setup_setInstallPrefix()
	{
		QVERIFY(TestUtil::runProgram("./soqute_cmd", QStringList() << "config"
																   << "--set"
																   << "install-prefix"
																   << dir.absolutePath()));
	}

	void install()
	{
		QVERIFY(TestUtil::runProgram(
			"./soqute_cmd", QStringList() << "install"
										  << "--silent"
										  << "testpackage/1.0.0#generic-test-platform"));
		installDir.refresh();
		QVERIFY2(installDir.exists(), "ERROR: Root install directory was not created");
		fromDir.cd("packages");
		fromDir.cd("full");
		fromDir.refresh();
		QVERIFY(TestUtil::compareDirectories(installDir, fromDir));
	}

	void removeFirst()
	{
		QVERIFY(TestUtil::runProgram(
			"./soqute_cmd", QStringList() << "remove"
										  << "--silent"
										  << "testpackage1/1.0.0#generic-test-platform"));
		fromDir.cdUp();
		fromDir.cd("package2");
		fromDir.refresh();
		QVERIFY(TestUtil::compareDirectories(installDir, fromDir));
	}
	void removeSecond()
	{
		QVERIFY(TestUtil::runProgram(
			"./soqute_cmd", QStringList() << "remove"
										  << "--silent"
										  << "testpackage2/1.0.0#generic-test-platform"));
		installDir.refresh();
		QVERIFY(installDir.entryList(QDir::NoDotAndDotDot).size() == 0);
	}

	void cleanupTestCase()
	{
		out << "Restoring backup of configuration file..." << endl << flush;
		QDir::home().remove(configFile);
		QDir::home().rename(configFile + ".backup", configFile);

		out << "Cleaning up..." << endl << flush;
		if (dir.exists()) {
			dir.removeRecursively();
		}
	}
};

QTEST_GUILESS_MAIN(BlackBox_InstallRemove)

#include "tst_blackbox_install_remove.moc"
