#include "jsinstaller.h"

#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QtQml/QJSEngine>
#include <cstdio>

#include "package.h"
#include "karchive.h"
#include "ktar.h"
#include "configurationhandler.h"
#include "util_core.h"

Q_DECLARE_METATYPE(const KArchiveDirectory*)

class FileSystemModifier : public QObject
{
	Q_OBJECT
public:
	FileSystemModifier(QJSEngine* engine) : QObject(engine), m_engine(engine)
	{
	}

private:
	const KArchiveEntry* entryFromPath(const KArchiveDirectory* base, const QString& path)
	{
		if (path.isEmpty() || (path == "/")) {
			return base;
		} else {
            const KArchiveEntry* entry = base->entry(path.right(path.indexOf('/')));
			if (entry->isFile()) {
				return entry;
			} else {
				return entryFromPath(static_cast<const KArchiveDirectory*>(entry), path.right(path.indexOf('/')));
			}
		}
	}

	QJSEngine* m_engine;

public slots:
	void mkdir(const QString& path) { mkpath(path); }
	void mkpath(const QString& path)
	{
        Util::ensureExists(path);
	}

	void install(const QString& source, const QString& destination)
	{
        const KArchiveDirectory* root = m_engine->globalObject().property("_rootArchiveDirectory").toVariant().value<const KArchiveDirectory*>();
		const KArchiveEntry* entry = root->entry(source);
        Util::installArchiveEntry(entry, destination);
	}
	void remove(const QString& entry)
	{
		QFileInfo info(entry);
		QDir dir = info.absoluteDir();
        if (info.isDir()) {
            Util::removeDirectoryRecursive(QDir(info.absoluteFilePath()));
        } else {
           dir.remove(info.fileName());
        }
	}
};

class OperatingSystem : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int processor READ processor)
	Q_PROPERTY(QString os READ os)

public:
	OperatingSystem(QJSEngine* engine) : QObject(engine), m_engine(engine)
	{
	}

	int processor() const
	{
		return
		#if defined(Q_PROCESSOR_X86_64)
				64
		#elif defined(Q_PROCESSOR_X86_32)
				32
		#else
		#error Your operating system is not supported
		#endif
				;
	}

	QString os() const
	{
		return
		#if defined(Q_OS_LINUX)
				"linux"
		#elif defined(Q_OS_WIN32)
				"win32"
		#else
		#error Your operating system is not supported
		#endif
				;
	}

private:
	QJSEngine* m_engine;

public slots:
};

class Messenger : public QObject
{
	Q_OBJECT

public:
	Messenger(QJSEngine* engine) : QObject(engine), m_engine(engine)
	{
	}

private:
	QJSEngine* m_engine;

public slots:
	void message(const QString& msg)
	{
		emit qobject_cast<JSInstaller*>(m_engine->globalObject().property("_jsinstaller").toQObject())->message(msg);
	}
};

class SystemModifier : public QObject
{
	Q_OBJECT

public:
	SystemModifier(QJSEngine* engine) : QObject(engine), m_engine(engine)
	{
	}

private:
	QJSEngine* m_engine;

public slots:
	void appendToPath(const QString& path, const bool append)
	{
#if defined(Q_OS_LINUX)
		QFile f(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first() + "/.bashrc");
		if (f.open(QFile::ReadOnly | QFile::Append)) {
			// TODO more intelligent (append to existing exports)
			f.write("\nexport PATH=");
			f.write(append ? "$PATH" : qPrintable(path));
			f.write(":");
			f.write(append ? qPrintable(path) : "$PATH");
			f.write("\n");
			f.close();
		}
#elif defined(Q_OS_WIN32)
#error Win32 is not supported yet
#else
#error Your operating system is not supported yet
#endif
	}
};

class Paths : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString tempPath READ tempPath)
	Q_PROPERTY(QString installPath READ installPath)

public:
	Paths(QJSEngine* engine) : QObject(engine), m_engine(engine)
	{
	}

	QString tempPath() const { return QStandardPaths::standardLocations(QStandardPaths::TempLocation).first() + "/soqute"; }
	QString installPath() const { return Util::installationRoot(m_version, m_platform); }

	void setData(const QString& version, const QString& platform)
	{
		m_version = version;
		m_platform = platform;
	}

private:
	QJSEngine* m_engine;
	QString m_version;
	QString m_platform;

public slots:
};

JSInstaller::JSInstaller(QObject *parent) :
	AbstractInstaller(parent), m_engine(new QJSEngine(this))
{
	setupEngine();
}

void JSInstaller::install(const Package *package, const QString &fileName, QString *errorString)
{
	emit installPackageBegin(package);

	// make sure we can use errorString
	// TODO this is a hack
	if (!errorString) {
		errorString = new QString();
	}

	KArchive* archive = new KTar(fileName);
	if (!archive->open(QIODevice::ReadOnly)) {
		*errorString = tr("Couldn't open archive");
		emit error(*errorString);
		exit(1);
		return;
	}

	const KArchiveDirectory* directory = archive->directory();

	const QStringList entries = directory->entries();
	if (!entries.contains("install.js")) {
		*errorString = tr("Archive does not contain installation script");
		emit error(*errorString);
		exit(1);
		return;
	}

	setupEngineForPackage(package);

	Util::ensureExists(Util::installationRoot(package->version(), package->platform()));
	m_engine->globalObject().setProperty("_rootArchiveDirectory", m_engine->toScriptValue(QVariant::fromValue(directory)));

	const KArchiveFile* scriptFile = static_cast<const KArchiveFile*>(directory->entry("install.js"));

	const QString program = QString::fromUtf8(scriptFile->data());

	QJSValue function = m_engine->evaluate(program, "install.js");

	if (function.isError()) {
		*errorString = tr("Error reading installation script: %1").arg(function.toString());
		emit error(*errorString);
		exit(1);
		return;
	}

    Util::ensureExists(Util::removalScriptsDirectory().absolutePath());
    static_cast<const KArchiveFile*>(directory->entry("remove.js"))->copyTo(QDir::tempPath());
    QDir::temp().rename("remove.js", Util::removalScriptsDirectory().absoluteFilePath(QString("%1-%2-%3-remove.js").arg(package->id(), package->version(), package->platform())));

	delete archive;

	emit installPackageEnd(package);

	return;
}

void JSInstaller::remove(const Package *package, const QString &fileName, QString *errorString)
{
	emit removePackageBegin(package);

	// make sure we can use errorString
	// TODO this is a hack
	if (!errorString) {
		errorString = new QString();
	}

	QFile file(fileName);
	if (!file.open(QFile::ReadOnly)) {
		*errorString = tr("Unable to open removal script: %1").arg(file.errorString());
		emit error(*errorString);
		exit(1);
		return;
	}

	setupEngineForPackage(package);

	QJSValue function = m_engine->evaluate(QString::fromUtf8(file.readAll()), fileName);

	if (function.isError()) {
		*errorString = tr("Error reading removal script: %1").arg(function.toString());
		emit error(*errorString);
		emit message(tr("Warning: Because the removal of the package failed might now be broken, and you might want to reinstall it."));
		exit(1);
		return;
	}

	emit removePackageEnd(package);
}

void JSInstaller::setupEngine()
{
	m_engine->globalObject().setProperty("Paths", m_engine->newQObject(new Paths(m_engine)));
	m_engine->globalObject().setProperty("OperatingSystem", m_engine->newQObject(new OperatingSystem(m_engine)));
	m_engine->globalObject().setProperty("FileSystem", m_engine->newQObject(new FileSystemModifier(m_engine)));
	m_engine->globalObject().setProperty("System", m_engine->newQObject(new SystemModifier(m_engine)));
	m_engine->globalObject().setProperty("Messenger", m_engine->newQObject(new Messenger(m_engine)));
	m_engine->globalObject().setProperty("_jsinstaller", m_engine->newQObject(this));
}

void JSInstaller::setupEngineForPackage(const Package *package)
{
	Paths* paths = qobject_cast<Paths*>(m_engine->globalObject().property("Paths").toQObject());
	paths->setData(package->version(), package->platform());
	m_engine->globalObject().setProperty("id", m_engine->toScriptValue(package->id()));
	m_engine->globalObject().setProperty("version", m_engine->toScriptValue(package->version()));
	m_engine->globalObject().setProperty("platform", m_engine->toScriptValue(package->platform()));
}

#include "jsinstaller.moc"
