#include "testutil.h"

#include <iostream>
#include <QDir>
#include <QCryptographicHash>
#include <QProcess>
#include <QCoreApplication>

QProcessEnvironment TestUtil::env = QProcessEnvironment::systemEnvironment();
QTextStream out(stdout);

bool TestUtil::confirm()
{
	char c;
	std::cin >> c;
	return c == 'y';
}

TestUtil::Output::Output(QIODevice *device) : QObject(), m_device(device)
{
	connect(m_device, &QIODevice::readyRead, this, &Output::dataRead);
}
void TestUtil::Output::dataRead()
{
	while (m_device->canReadLine()) {
		out << "    " << m_device->readLine();
	}
	out << flush;
}

bool TestUtil::compareFiles(const QString &f1, const QString &f2)
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
bool TestUtil::compareDirectories(const QDir &d1, const QDir &d2)
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
bool TestUtil::compareEntry(const QFileInfo &i1, const QFileInfo &i2)
{
	if (i1.isFile() && i2.isFile()) {
		return compareFiles(i1.absoluteFilePath(), i2.absoluteFilePath());
	} else if (i1.isDir() && i2.isDir()) {
		return compareDirectories(i1.absoluteFilePath(), i2.absoluteFilePath());
	} else {
		return false;
	}
}


bool TestUtil::runProgram(const QString &program, const QStringList &arguments)
{
	out << "  Running " << program << " " << arguments.join(' ') << endl << flush;
	QProcess proc;
	proc.setProgram(program);
	proc.setWorkingDirectory(qApp->applicationDirPath());
	proc.setArguments(arguments);
	proc.setProcessEnvironment(TestUtil::env);
	proc.setProcessChannelMode(QProcess::MergedChannels);
	TestUtil::Output output(&proc);
	proc.start();
	proc.waitForStarted(-1);
	proc.waitForFinished(-1);
	if (proc.exitCode() != QProcess::NormalExit || proc.error() != QProcess::UnknownError) {
		return false;
	}
	return true;
}
