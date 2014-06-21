#include "filegetters.h"

#include <QDebug>

namespace GetFiles
{
namespace Internal
{
QStringList standardLibraryFiles(const QString &infix)
{
	QStringList out;
	out << "lib###.la"
		<< "lib###.prl"
		<< "lib###.so"
		<< "pkgconfig/###.pc"
		<< "cmake/###";
	return out.replaceInStrings("###", infix);
}
QStringList standardDocFiles(const QString &infix)
{
	QStringList out;
	out << "###"
		<< "###.qch";
	return out.replaceInStrings("###", infix);
}
}

using namespace Internal;

QString version;

QMap<QString, QStringList> binaryFiles()
{
	static QMap<QString, QStringList> binaryFiles;
	binaryFiles["qtcore"] = QStringList() << "qmake"
										  << "moc"
										  << "rcc";
	binaryFiles["qtqml"] = QStringList() << "qmlbundle"
										 << "qmlmin"
										 << "qmlplugindump"
										 << "qmlprofiler"
										 << "qmlscene"
										 << "qmltestrunner"
										 << "qmlviewer";
	binaryFiles["qtquick"] = QStringList() << "qml2puppet";
	binaryFiles["qtwidgets"] = QStringList() << "pixeltool"
											 << "uic";
	binaryFiles["qtdbus"] = QStringList() << "qdbus"
										  << "qdbuscpp2xml"
										  << "qdbusviewer"
										  << "qdbusxml2cpp";
	binaryFiles["qtquick1"] = QStringList() << "qml1plugindump";
	binaryFiles["qtlinguisttools"] = QStringList() << "lconvert"
												   << "linguist"
												   << "lrelease"
												   << "lupdate";
	binaryFiles["qtxmlpatterns"] = QStringList() << "xmlpatterns"
												 << "xmlpatternsvalidator";
	binaryFiles["qtuitools"] = QStringList() << "designer";
	binaryFiles["qthelp"] = QStringList() << "assistant"
										  << "qcollectiongenerator"
										  << "qdoc"
										  << "qhelpconverter"
										  << "qhelpgenerator";
	return binaryFiles;
}
QMap<QString, QStringList> docFiles()
{
	static QMap<QString, QStringList> docFiles;
	docFiles["qtcore"] = QStringList() << standardDocFiles("qtcore") << "global";
	docFiles["qtgui"] = QStringList() << standardDocFiles("qtgui");
	docFiles["qtmultimedia"] = QStringList() << standardDocFiles("qtmultimedia");
	docFiles["qtmultimediawidgets"] = QStringList() << standardDocFiles("qtmultimediawidgets");
	docFiles["qtnetwork"] = QStringList() << standardDocFiles("qtnetwork");
	docFiles["qtqml"] = QStringList() << standardDocFiles("qtqml");
	docFiles["qtquick"] = QStringList() << standardDocFiles("qtquick");
	docFiles["qtquickcontrols"] = QStringList() << standardDocFiles("qtquickcontrols")
												<< standardDocFiles("qtquickcontrolsstyles");
	docFiles["qtquickdialogs"] = QStringList() << standardDocFiles("qtquickdialogs");
	docFiles["qtquicklayouts"] = QStringList() << standardDocFiles("qtquicklayouts");
	docFiles["qtsql"] = QStringList() << standardDocFiles("qtsql");
	docFiles["qttest"] = QStringList() << standardDocFiles("qttestlib");
	docFiles["qtwebkit"] = QStringList() << standardDocFiles("qtwebkit");
	docFiles["qtwebkitexamples"] = QStringList() << standardDocFiles("qtwebkitexamples");
	docFiles["qtwidgets"] = QStringList() << standardDocFiles("qtwidgets");
	docFiles["qtactiveqt"] = QStringList() << standardDocFiles("activeqt");
	docFiles["qtconcurrent"] = QStringList() << standardDocFiles("qtconcurrent");
	docFiles["qtdbus"] = QStringList() << standardDocFiles("qtdbus");
	docFiles["qtgraphicaleffects"] = QStringList() << standardDocFiles("qtgraphicaleffects");
	docFiles["qtimageformats"] = QStringList() << standardDocFiles("qtimageformats");
	docFiles["qtopengl"] = QStringList() << standardDocFiles("qtopengl");
	docFiles["qtprintsupport"] = QStringList() << standardDocFiles("qtprintsupport");
	docFiles["qtscript"] = QStringList() << standardDocFiles("qtscript");
	docFiles["qtscripttools"] = QStringList() << standardDocFiles("qtscripttools");
	docFiles["qtsensors"] = QStringList() << standardDocFiles("qtsensors");
	docFiles["qtserialport"] = QStringList() << standardDocFiles("qtserialport");
	docFiles["qtsvg"] = QStringList() << standardDocFiles("qtsvg");
	docFiles["qtx11extras"] = QStringList() << standardDocFiles("qtx11extras");
	docFiles["qtxml"] = QStringList() << standardDocFiles("qtxml");
	docFiles["qtxmlpatterns"] = QStringList() << standardDocFiles("qtxmlpatterns");
	docFiles["qtdesigner"] = QStringList() << standardDocFiles("qtdesigner");
	docFiles["qtuitools"] = QStringList() << standardDocFiles("qtuitools");
	docFiles["qthelp"] = QStringList() << standardDocFiles("qthelp");
	docFiles["qtlinguisttools"] = QStringList() << standardDocFiles("qtlinguist");
	return docFiles;
}
QMap<QString, QStringList> exampleFiles()
{
	static QMap<QString, QStringList> exampleFiles;
	exampleFiles["qtcore"] = QStringList() << "examples.pro"
										   << "aggregate"
										   << "ipc"
										   << "qmake"
										   << "threads"
										   << "tools";
	exampleFiles["qtgui"] = QStringList() << "gui";
	exampleFiles["qtmultimedia"] = QStringList() << "multimedia";
	exampleFiles["qtmultimediawidgets"] = QStringList() << "multimediawidgets";
	exampleFiles["qtnetwork"] = QStringList() << "network";
	exampleFiles["qtqml"] = QStringList() << "qml"
										  << "qmltest";
	exampleFiles["qtquick"] = QStringList() << "quick";
	exampleFiles["qtsql"] = QStringList() << "sql";
	exampleFiles["qttest"] = QStringList() << "qtestlib";
	exampleFiles["qtwebkit"] = QStringList() << "webkit"
											 << "webkitqml";
	exampleFiles["qtwebkitwidgets"] = QStringList() << "webkitwidgets";
	exampleFiles["qtwidgets"] = QStringList() << "widgets"
											  << "embedded"
											  << "gestures"
											  << "touch";
	exampleFiles["qtactiveqt"] = QStringList() << "activeqt";
	exampleFiles["qtconcurrent"] = QStringList() << "qtconcurrent";
	exampleFiles["qtdbus"] = QStringList() << "dbus";
	exampleFiles["qtopengl"] = QStringList() << "opengl";
	exampleFiles["qtquick1"] = QStringList() << "declarative"
											 << "demos";
	exampleFiles["qtscript"] = QStringList() << "script";
	exampleFiles["qtsensors"] = QStringList() << "sensors";
	exampleFiles["qtserialport"] = QStringList() << "serialport";
	exampleFiles["qtsvg"] = QStringList() << "svg";
	exampleFiles["qtxml"] = QStringList() << "xml";
	exampleFiles["qtxmlpatterns"] = QStringList() << "xmlpatterns";
	exampleFiles["qtdesigner"] = QStringList() << "designer";
	exampleFiles["qtuitools"] = QStringList() << "uitools";
	exampleFiles["qthelp"] = QStringList() << "help"
										   << "assistant";
	exampleFiles["qtplatformsupport"] = QStringList() << "qpa";
	return exampleFiles;
}
QMap<QString, QStringList> importsFiles()
{
	static QMap<QString, QStringList> importsFiles;
	importsFiles["qtquick"] = QStringList() << "builtins.qmltypes"
											<< "Qt";
	importsFiles["qtwebkit"] = QStringList() << "QtWebKit";
	return importsFiles;
}
QMap<QString, QStringList> libExecFiles()
{
	static QMap<QString, QStringList> libexecFiles;
	libexecFiles["qtwebkit"] = QStringList() << "QtWebProcess"
											 << "QtWebPluginProcess";
	return libexecFiles;
}
QMap<QString, QStringList> mkSpecFiles()
{
	static QMap<QString, QStringList> mkspecFiles;
	mkspecFiles["qtcore"] = QStringList() << "aix-g++"
										  << "aix-g++-64"
										  << "aix-xlc"
										  << "aix-xlc-64"
										  << "android-g++"
										  << "blackberry-armv7le-qcc"
										  << "blackberry-x86-qcc"
										  << "common"
										  << "cygwin-g++"
										  << "darwin-g++"
										  << "devices"
										  << "features"
										  << "freebsd-g++"
										  << "freebsd-g++46"
										  << "freebsd-icc"
										  << "hpux-acc"
										  << "hpux-acc-64"
										  << "hpux-acc-o64"
										  << "hpux-g++"
										  << "hpux-g++-64"
										  << "hpuxi-acc-32"
										  << "hpuxi-acc-64"
										  << "hpuxi-g++-64"
										  << "hurd-g++"
										  << "irix-cc"
										  << "irix-cc-64"
										  << "irix-g++"
										  << "irix-g++-64"
										  << "linux-arm-gnueabi-g++"
										  << "linux-clang"
										  << "linux-cxx"
										  << "linux-g++"
										  << "linux-g++-32"
										  << "linux-g++-64"
										  << "linux-g++-maemo"
										  << "linux-icc"
										  << "linux-icc-32"
										  << "linux-icc-64"
										  << "linux-kcc"
										  << "linux-llvm"
										  << "linux-lsb-g++"
										  << "linux-pgcc"
										  << "lynxos-g++"
										  << "macx-clang"
										  << "macx-clang-32"
										  << "macx-clang-libc++"
										  << "macx-clang-libc++-32"
										  << "macx-g++"
										  << "macx-g++-32"
										  << "macx-g++40"
										  << "macx-g++42"
										  << "macx-icc"
										  << "macx-llvm"
										  << "macx-xcode"
										  << "modules"
										  << "netbsd-g++"
										  << "openbsd-g++"
										  << "qconfig.pri"
										  << "qdevice.pri"
										  << "qmodule.pri"
										  << "qnx-armv7le-qcc"
										  << "qnx-x86-qcc"
										  << "sco-cc"
										  << "sco-g++"
										  << "solaris-cc"
										  << "solaris-cc-64"
										  << "solaris-cc-64-stlport"
										  << "solaris-cc-stlport"
										  << "solaris-g++"
										  << "solaris-g++-64"
										  << "tru64-cxx"
										  << "tru64-g++"
										  << "unixware-cc"
										  << "unixware-g++"
										  << "unsupported"
										  << "win32-g++"
										  << "win32-icc"
										  << "win32-msvc2005"
										  << "win32-msvc2008"
										  << "win32-msvc2010"
										  << "win32-msvc2012"
										  << "wince50standard-armv4i-msvc2005"
										  << "wince50standard-armv4i-msvc2008"
										  << "wince50standard-mipsii-msvc2005"
										  << "wince50standard-mipsii-msvc2008"
										  << "wince50standard-mipsiv-msvc2005"
										  << "wince50standard-mipsiv-msvc2008"
										  << "wince50standard-sh4-msvc2005"
										  << "wince50standard-sh4-msvc2008"
										  << "wince50standard-x86-msvc2005"
										  << "wince50standard-x86-msvc2008"
										  << "wince60standard-armv4i-msvc2005"
										  << "wince60standard-x86-msvc2005"
										  << "wince70embedded-armv4i-msvc2008"
										  << "wince70embedded-x86-msvc2008"
										  << "wincewm50pocket-msvc2005"
										  << "wincewm50pocket-msvc2008"
										  << "wincewm50smart-msvc2005"
										  << "wincewm50smart-msvc2008"
										  << "wincewm60professional-msvc2005"
										  << "wincewm60professional-msvc2008"
										  << "wincewm60standard-msvc2005"
										  << "wincewm60standard-msvc2008"
										  << "wincewm65professional-msvc2005"
										  << "wincewm65professional-msvc2008";
	return mkspecFiles;
}
QMap<QString, QStringList> phrasebookFiles()
{
	static QMap<QString, QStringList> phrasebookFiles;
	phrasebookFiles["qtlinguisttools"] = QStringList() << "danish.qph"
													   << "dutch.qph"
													   << "finnish.qph"
													   << "french.qph"
													   << "german.qph"
													   << "hungarian.qph"
													   << "italian.qph"
													   << "japanese.qph"
													   << "norwegian.qph"
													   << "polish.qph"
													   << "russian.qph"
													   << "spanish.qph"
													   << "swedish.qph";
	return phrasebookFiles;
}
QMap<QString, QStringList> qmlFiles()
{
	static QMap<QString, QStringList> qmlFiles;
	qmlFiles["qtmultimedia"] = QStringList() << "QtAudioEngine"
											 << "QtMultimedia";
	qmlFiles["qtqml"] = QStringList() << "Qt"
									  << "QtQml";
	qmlFiles["qtquick"] = QStringList() << "QtQuick/XmlListModel"
										<< "QtQuick/LocalStorage"
										<< "QtQuick/Particles.2"
										<< "QtQuick.2";
	qmlFiles["qtquickcontrols"] = QStringList() << "QtQuick/Controls"
												<< "QtQuick/Window.2"
												<< "QtQuick/PrivateWidgets";
	qmlFiles["qtquicklayouts"] = QStringList() << "QtQuick/Layouts";
	qmlFiles["qtquickdialogs"] = QStringList() << "QtQuick/Dialogs";
	qmlFiles["qttest"] = QStringList() << "QtTest";
	qmlFiles["qtwebkit"] = QStringList() << "QtWebKit";
	qmlFiles["qtgraphicaleffects"] = QStringList() << "QtGraphicalEffects";
	qmlFiles["qtsensors"] = QStringList() << "QtSensors";
	return qmlFiles;
}
QMap<QString, QStringList> translationFiles()
{
	static QMap<QString, QStringList> translationFiles;
	translationFiles["qtcore"] = QStringList() << "qt_*.qm"
											   << "qtbase_*.qm"
											   << "qtconfig_*.qm";
	translationFiles["qtmultimedia"] = QStringList() << "qtmultimedia_*.qm";
	translationFiles["qtqml"] = QStringList() << "qmlviewer*.qm";
	translationFiles["qtquick"] = QStringList() << "qtdeclarative_*.qm";
	translationFiles["qtquick1"] = QStringList() << "qtquick1_*.qm";
	translationFiles["qtscript"] = QStringList() << "qtscript_*.qm";
	translationFiles["qtxmlpatterns"] = QStringList() << "qtxmlpatterns_*.qm";
	translationFiles["qtlinguisttools"] = QStringList() << "linguist_*.qm";
	translationFiles["qtdesigner"] = QStringList() << "designer_*.qm";
	translationFiles["qthelp"] = QStringList() << "assistant_*.qm"
											   << "qt_help_*.qm";
	return translationFiles;
}
QMap<QString, QStringList> includeFolders()
{
	static QMap<QString, QStringList> includeFolders;
	includeFolders["qtcore"] = QStringList() << "QtCore";
	includeFolders["qtgui"] = QStringList() << "QtGui"
											<< "QtOpenGL"
											<< "QtOpenGLExtensions";
	includeFolders["qtmultimedia"] = QStringList() << "QtMultimedia"
												   << "QtMultimediaQuick_p";
	includeFolders["qtmultimediawidgets"] = QStringList() << "QtMultimediaWidgets";
	includeFolders["qtnetwork"] = QStringList() << "QtNetwork";
	includeFolders["qtqml"] = QStringList() << "QtQml";
	includeFolders["qtquick"] = QStringList() << "QtQuick"
											  << "QtQuickParticles";
	includeFolders["qtquicktest"] = QStringList() << "QtQuickTest";
	includeFolders["qtsql"] = QStringList() << "QtSql";
	includeFolders["qttest"] = QStringList() << "QtTest";
	includeFolders["qtwebkit"] = QStringList() << "QtWebKit";
	includeFolders["qtwebkitwidgets"] = QStringList() << "QtWebKitWidgets";
	includeFolders["qtwidgets"] = QStringList() << "QtWidgets";
	includeFolders["qtconcurrent"] = QStringList() << "QtConcurrent";
	includeFolders["qtdbus"] = QStringList() << "QtDBus";
	includeFolders["qtprintsupport"] = QStringList() << "QtPrintSupport";
	includeFolders["qtquick1"] = QStringList() << "QtDeclarative";
	includeFolders["qtscript"] = QStringList() << "QtScript";
	includeFolders["qtscripttools"] = QStringList() << "QtScriptTools";
	includeFolders["qtsensors"] = QStringList() << "QtSensors";
	includeFolders["qtserialport"] = QStringList() << "QtSerialPort";
	includeFolders["qtsvg"] = QStringList() << "QtSvg";
	includeFolders["qtx11extras"] = QStringList() << "QtX11Extras";
	includeFolders["qtxml"] = QStringList() << "QtXml";
	includeFolders["qtxmlpatterns"] = QStringList() << "QtXmlPatterns";
	includeFolders["qtdesigner"] = QStringList() << "QtDesigner"
												 << "QtDesignerComponents";
	includeFolders["qtuitools"] = QStringList() << "QtUiTools";
	includeFolders["qthelp"] = QStringList() << "QtHelp";
	return includeFolders;
}
QMap<QString, QStringList> libFiles()
{
	static QMap<QString, QStringList> libraryFiles;
	libraryFiles["qtcore"] =
		QStringList() << standardLibraryFiles("Qt5Core") << standardLibraryFiles("Qt5CLucene")
					  << standardLibraryFiles("Qt5Bootstrap") << "libQt5Bootstrap.a"
					  << "cmake/Qt5";
	libraryFiles["qtgui"] = QStringList() << standardLibraryFiles("Qt5Gui")
										  << standardLibraryFiles("Qt5OpenGL")
										  << standardLibraryFiles("Qt5OpenGLExtensions")
										  << "libQt5OpenGLExtensions.a";
	libraryFiles["qtmultimedia"] =
		QStringList() << standardLibraryFiles("Qt5Multimedia")
					  << standardLibraryFiles("Qt5MultimediaQuick_p");
	libraryFiles["qtmultimediawidgets"] = QStringList()
										  << standardLibraryFiles("Qt5MultimediaWidgets");
	libraryFiles["qtnetwork"] = QStringList() << standardLibraryFiles("Qt5Network");
	libraryFiles["qtqml"] = QStringList() << standardLibraryFiles("Qt5Qml")
										  << standardLibraryFiles("Qt5QmlDevTools")
										  << "libQt5QmlDevTools.a";
	libraryFiles["qtquick"] = QStringList() << standardLibraryFiles("Qt5Quick")
											<< standardLibraryFiles("Qt5QuickParticles");
	libraryFiles["qtquicktest"] = QStringList() << standardLibraryFiles("Qt5QuickTest");
	libraryFiles["qtsql"] = QStringList() << standardLibraryFiles("Qt5Sql");
	libraryFiles["qttest"] = QStringList() << standardLibraryFiles("Qt5Test");
	libraryFiles["qtwebkit"] = QStringList() << standardLibraryFiles("Qt5WebKit");
	libraryFiles["qtwebkitwidgets"] = QStringList() << standardLibraryFiles("Qt5WebKitWidgets");
	libraryFiles["qtwidgets"] = QStringList() << standardLibraryFiles("Qt5Widgets");
	libraryFiles["qtconcurrent"] = QStringList() << standardLibraryFiles("Qt5Concurrent");
	libraryFiles["qtdbus"] = QStringList() << standardLibraryFiles("Qt5DBus");
	libraryFiles["qtprintsupport"] = QStringList() << standardLibraryFiles("Qt5PrintSupport");
	libraryFiles["qtquick1"] = QStringList() << standardLibraryFiles("Qt5Declarative");
	libraryFiles["qtscript"] = QStringList() << standardLibraryFiles("Qt5Script");
	libraryFiles["qtscripttools"] = QStringList() << standardLibraryFiles("Qt5ScriptTools");
	libraryFiles["qtsensors"] = QStringList() << standardLibraryFiles("Qt5Sensors");
	libraryFiles["qtserialport"] = QStringList() << standardLibraryFiles("Qt5SerialPort");
	libraryFiles["qtsvg"] = QStringList() << standardLibraryFiles("Qt5Svg");
	libraryFiles["qtx11extras"] = QStringList() << standardLibraryFiles("Qt5X11Extras");
	libraryFiles["qtxml"] = QStringList() << standardLibraryFiles("Qt5Xml");
	libraryFiles["qtxmlpatterns"] = QStringList() << standardLibraryFiles("Qt5XmlPatterns");
	libraryFiles["qtdesigner"] = QStringList() << standardLibraryFiles("Qt5Designer")
											   << standardLibraryFiles("Qt5DesignerComponents");
	libraryFiles["qtuitools"] = QStringList() << standardLibraryFiles("Qt5UiTools")
											  << "libQt5UiTools.a";
	libraryFiles["qthelp"] = QStringList() << standardLibraryFiles("Qt5Help");
	libraryFiles["qtjsbackend"] = QStringList() << standardLibraryFiles("Qt5V8");
	libraryFiles["qtplatformsupport"] = QStringList()
										<< standardLibraryFiles("Qt5PlatformSupport")
										<< "libQt5PlatformSupport.a";
	libraryFiles["qtlinguisttools"] = QStringList() << "cmake/Qt5LinguistTools";
	return libraryFiles;
}
QMap<QString, QStringList> pluginNames()
{
	static QMap<QString, QStringList> pluginNames;
	pluginNames["qtcore"] = QStringList() << "generic"
										  << "platforminputcontexts"
										  << "platforms";
	pluginNames["qtmultimedia"] = QStringList() << "audio"
												<< "mediaservice"
												<< "playlistformats";
	pluginNames["qtnetwork"] = QStringList() << "bearer";
	pluginNames["qtqml"] = QStringList() << "qmltooling";
	pluginNames["qtsql"] = QStringList() << "sqldrivers";
	pluginNames["qtwebkitwidgets"] = QStringList() << "designer/libqwebview.so";
	pluginNames["qtwidgets"] = QStringList() << "platformthemes";
	pluginNames["qtimageformats"] = QStringList() << "imageformats";
	pluginNames["qtprintsupport"] = QStringList() << "printsupport";
	pluginNames["qtquick1"] = QStringList() << "designer/libqdeclarativeview.so"
											<< "qml1tooling";
	pluginNames["qtsensors"] = QStringList() << "sensorgestures"
											 << "sensors";
	pluginNames["qtsvg"] = QStringList() << "iconengines/libsvgicon.so";
	pluginNames["qtaccessability"] = QStringList() << "accessible";
	return pluginNames;
}

QMap<QString, QString> descriptions()
{
	static QMap<QString, QString> descs;
	descs["qtcore"] = "Core non-graphical classes used by other modules.";
	descs["qtgui"] =
		"Base classes for graphical user interface (GUI) components. Includes OpenGL.";
	descs["qtmultimedia"] = "Classes for audio, video, radio and camera functionality.";
	descs["qtmultimediawidgets"] =
		"Widget-based classes for implementing multimedia functionality.";
	descs["qtnetwork"] = "Classes to make network programming easier and more portable.";
	descs["qtqml"] = "Classes for QML and JavaScript languages.";
	descs["qtquick"] =
		"A declarative framework for building highly dynamic applications with custom user "
		"interfaces.";
	descs["qtsql"] = "Classes for database integration using SQL.";
	descs["qttest"] = "Classes for unit testing Qt applications and libraries.";
	descs["qtwebkit"] =
		"Classes for a WebKit2 based implementation and a new QML API. See also Qt WebKit "
		"Widgets in the add-on modules.";
	descs["qtwebkitwidgets"] = "WebKit1 and QWidget-based classes from Qt 4.";
	descs["qtwidgets"] = "Classes to extend Qt GUI with C++ widgets.";
	descs["qtconcurrent"] =
		"Classes for writing multi-threaded programs without using low-level threading "
		"primitives.";
	descs["qtdbus"] = "Classes for inter-process communication over the D-Bus protocol.";
	descs["qtprintsupport"] = "Classes to make printing easier and more portable.";
	descs["qtquick1"] =
		"Qt Declarative is provided for Qt 4 compatibility. The documentation is available "
		"through the Qt 4.8 Qt Quick documentation.";
	descs["qtscript"] =
		"Classes for making Qt applications scriptable. Provided for Qt 4.x compatibility. "
		"Please use the QJS* classes in the QtQml module for new code.";
	descs["qtscripttools"] = "Additional components for applications that use Qt Script.";
	descs["qtsensors"] = "Provides access to sensor hardware and motion gesture recognition.";
	descs["qtserialport"] = "Provides access to hardware and virtual serial ports.";
	descs["qtsvg"] = "Classes for displaying the contents of SVG files.";
	descs["qtx11extras"] = "Provides specific APIs for X11.";
	descs["qtxml"] =
		"C++ implementations of SAX and DOM. Note: Deprecated, please use QXmlStreamReader and "
		"QXmlStreamWriter for new functionality.";
	descs["qtxmlpatterns"] = "Support for XPath, XQuery, XSLT and XML schema validation.";
	descs["qtdesigner"] = "Classes for extending Qt Designer.";
	descs["qtuitools"] = "Classes for UI tools.";
	descs["qthelp"] = "Classes for online help.";
	descs["qtlinguisttools"] = "Tools and applications for translating Qt programs";
	descs["qtactiveqt"] = "Classes for applications which use ActiveX and COM";
	descs["qtgraphicaleffects"] = "Graphical effects for use with Qt Quick 2.";
	descs["qtimageformats"] = "Plugins for additional image formats: TIFF, MNG, TGA, WBMP.";
	descs["qtopengl"] =
		"OpenGL support classes. Note: Provided to ease porting from Qt 4.x. Please use the "
		"QOpenGL classes in QtGui for new code.";
	descs["qtquickcontrols"] =
		"Reusable Qt Quick based UI controls to create classic desktop-style user interfaces";
	descs["qtquicklayouts"] =
		"Layouts are items that are used to arrange Qt Quick 2 based items in the user "
		"interface.";
	descs["qtquickdialogs"] =
		"Reusable Qt Quick based UI controls to create classic desktop-style user interfaces";
	descs["qtwebkitexamples"] = "Examples of Qt WebKit usage";
	descs["qtplatformsupport"] = "Plugins for supporting different platforms";
	descs["qtjsbackend"] = "Backend used by Qt Qml";
	descs["qtaccessability"] = "Plugin for screen readers etc.";
	return descs;
}
QMap<QString, QStringList> dependencies()
{
	static QMap<QString, QStringList> deps;
	deps["qtcore"] = QStringList() << "";
	deps["qtgui"] = QStringList() << "qtcore";
	deps["qtmultimedia"] = QStringList() << "qtcore"
										 << "qtgui"
										 << "qtnetwork"
										 << "qtquick";
	deps["qtmultimediawidgets"] = QStringList() << "qtcore"
												<< "qtgui"
												<< "qtmultimedia"
												<< "qtwidgets"
												<< "qtopengl";
	deps["qtnetwork"] = QStringList() << "qtcore";
	deps["qtqml"] = QStringList() << "qtnetwork"
								  << "qtcore"
								  << "qtjsbackend";
	deps["qtquick"] = QStringList() << "qtqml"
									<< "qtnetwork"
									<< "qtgui"
									<< "qtcore"
									<< "qtjsbackend";
	deps["qtquicktest"] = QStringList() << "qtwidgets"
										<< "qtgui"
										<< "qtcore"
										<< "qtquick"
										<< "qtqml"
										<< "qttest"
										<< "qtjsbackend";
	deps["qtsql"] = QStringList() << "qtcore";
	deps["qttest"] = QStringList() << "qtcore";
	deps["qtwebkit"] = QStringList() << "qtcore"
									 << "qtquick"
									 << "qtqml"
									 << "qtnetwork"
									 << "qtgui"
									 << "qtsensors"
									 << "qtsql";
	deps["qtwebkitwidgets"] = QStringList() << "qtopengl"
											<< "qtprintsupport"
											<< "qtwebkit"
											<< "qtwidgets"
											<< "qtnetwork"
											<< "qtsensors"
											<< "qtgui"
											<< "qtcore";
	deps["qtwidgets"] = QStringList() << "qtgui"
									  << "qtcore";
	deps["qtconcurrent"] = QStringList() << "qtcore";
	deps["qtdbus"] = QStringList() << "qtcore";
	deps["qtprintsupport"] = QStringList() << "qtwidgets"
										   << "qtgui"
										   << "qtcore";
	deps["qtquick1"] = QStringList() << "qtxmlpatterns"
									 << "qtwidgets"
									 << "qtnetwork"
									 << "qtsql"
									 << "qtscript"
									 << "qtgui"
									 << "qtcore";
	deps["qtscript"] = QStringList() << "qtcore";
	deps["qtscripttools"] = QStringList() << "qtcore"
										  << "qtwidgets"
										  << "qtscript"
										  << "qtgui";
	deps["qtsensors"] = QStringList() << "qtcore";
	deps["qtserialport"] = QStringList() << "qtcore";
	deps["qtsvg"] = QStringList() << "qtwidgets"
								  << "qtgui"
								  << "qtcore";
	deps["qtx11extras"] = QStringList() << "qtwidgets"
										<< "qtgui"
										<< "qtcore";
	deps["qtxml"] = QStringList() << "qtcore";
	deps["qtxmlpatterns"] = QStringList() << "qtcore"
										  << "qtnetwork";
	deps["qtdesigner"] = QStringList() << "qtwidgets"
									   << "qtgui"
									   << "qtxml"
									   << "qtcore";
	deps["qtuitools"] = QStringList() << "qtcore"
									  << "qtwidgets"
									  << "qtgui";
	deps["qthelp"] = QStringList() << "qtwidgets"
								   << "qtgui"
								   << "qtcore"
								   << "qtsql"
								   << "qtnetwork";
	deps["qtlinguisttools"] = QStringList() << "qtprintsupport"
											<< "qtwidgets"
											<< "qtxml"
											<< "qtgui"
											<< "qcore";
	deps["qtactiveqt"] = QStringList();
	deps["qtgraphicaleffects"] = QStringList() << "qtquick";
	deps["qtimageformats"] = QStringList() << "qtcore"
										   << "qtgui"
										   << "qtsvg"
										   << "qtwidgets";
	deps["qtopengl"] = QStringList() << "qtcore"
									 << "qtwidgets"
									 << "qtgui";
	deps["qtquickcontrols"] = QStringList() << "qtquick";
	deps["qtquicklayouts"] = QStringList() << "qtquick";
	deps["qtquickdialogs"] = QStringList() << "qtquick";
	deps["qtwebkitexamples"] = QStringList() << "qtwebkit";
	deps["qtplatformsupport"] = QStringList() << "qtcore"
											  << "qtgui";
	deps["qtjsbackend"] = QStringList();
	deps["qtaccessability"] = QStringList() << "qtquick"
											<< "qtqml"
											<< "qtgui"
											<< "qtcore"
											<< "qtnetwork"
											<< "qtjsbackend"
											<< "qtwidgets";
	return deps;
}
QMap<QString, QMap<QString, QStringList>> nativeDependencies()
{
	static QMap<QString, QMap<QString, QStringList>> ndeps;
	ndeps["qtcore"]["apt"] = QStringList() << "libc6"
										   << "libgcc1"
										   << "libglib2.0-0"
										   << "libicu48"
										   << "libstdc++6"
										   << "zlib1g";
	ndeps["qtgui"]["apt"] = QStringList() << "fontconfig"
										  << "libc6"
										  << "libfontconfig1"
										  << "libfreetype6"
										  << "libglib2.0-0"
										  << "libjpeg8"
										  << "libpng12-0"
										  << "libstdc++6"
										  << "libudev0"
										  << "libx11-6"
										  << "libx11-xcb1"
										  << "libxcb-glx0"
										  << "libxcb-icccm4"
										  << "libxcb-image0"
										  << "libxcb-keysyms1"
										  << "libxcb-randr0"
										  << "libxcb-render-util0"
										  << "libxcb-render0"
										  << "libxcb-shape0"
										  << "libxcb-shm0"
										  << "libxcb-sync0"
										  << "libxcb-xfixes0"
										  << "libxcb1"
										  << "libxi6"
										  << "libxrender1"
										  << "zlib1g";
	ndeps["qtmultimedia"]["apt"] = QStringList() << "libc6"
												 << "libpulse0"
												 << "libstdc++6";
	ndeps["qtmultimediawidgets"]["apt"] = QStringList() << "libc6"
														<< "libstdc++6";
	ndeps["qtnetwork"]["apt"] = QStringList() << "libc6"
											  << "libstdc++6"
											  << "zlib1g";
	ndeps["qtqml"]["apt"] = QStringList() << "libc6"
										  << "libstdc++6";
	ndeps["qtquick"]["apt"] = QStringList() << "libc6"
											<< "libstdc++6";
	ndeps["qtsql"]["apt"] = QStringList() << "libc6"
										  << "libstdc++6";
	ndeps["qttest"]["apt"] = QStringList() << "libc6"
										   << "libgcc1"
										   << "libstdc++6";
	ndeps["qtwebkit"]["apt"] = QStringList() << "libc6"
											 << "libgcc1"
											 << "libglib2.0-0"
											 << "libgstreamer-plugins-base0.10-0"
											 << "libgstreamer0.10-0"
											 << "libicu48"
											 << "libjpeg8"
											 << "libpng12-0"
											 << "libsqlite3-0"
											 << "libstdc++6"
											 << "libx11-6"
											 << "libxml2"
											 << "libxslt1.1"
											 << "zlib1g";
	ndeps["qtwebkitwidgets"]["apt"] = QStringList();
	ndeps["qtwidgets"]["apt"] = QStringList() << "libc6"
											  << "libglib2.0-0"
											  << "libstdc++6"
											  << "libx11-6";
	ndeps["qtconcurrent"]["apt"] = QStringList() << "libc6"
												 << "libgcc1"
												 << "libstdc++6";
	ndeps["qtdbus"]["apt"] = QStringList() << "libc6"
										   << "libdbus-1-3"
										   << "libstdc++6";
	ndeps["qtprintsupport"]["apt"] = QStringList() << "libc6"
												   << "libstdc++6";
	ndeps["qtquick1"]["apt"] = QStringList() << "libc6"
											 << "libstdc++6";
	ndeps["qtscript"]["apt"] = QStringList() << "libc6"
											 << "libstdc++6";
	ndeps["qtscripttools"]["apt"] = QStringList() << "libc6"
												  << "libstdc++6";
	ndeps["qtsensors"]["apt"] = QStringList() << "libc6"
											  << "libstdc++6";
	ndeps["qtserialport"]["apt"] = QStringList();
	ndeps["qtsvg"]["apt"] = QStringList() << "libc6"
										  << "libstdc++6"
										  << "zlib1g";
	ndeps["qtx11extras"]["apt"] = QStringList();
	ndeps["qtxml"]["apt"] = QStringList() << "libc6"
										  << "libstdc++6";
	ndeps["qtxmlpatterns"]["apt"] = QStringList() << "libc6"
												  << "libgcc1"
												  << "libstdc++6";
	ndeps["qtdesigner"]["apt"] = QStringList() << "libc6"
											   << "libstdc++6";
	ndeps["qtuitools"]["apt"] = QStringList();
	ndeps["qthelp"]["apt"] = QStringList() << "libc6"
										   << "libgcc1"
										   << "libstdc++6";
	ndeps["qtlinguisttools"]["apt"] = QStringList();
	ndeps["qtactiveqt"]["apt"] = QStringList();
	ndeps["qtgraphicaleffects"]["apt"] = QStringList();
	ndeps["qtimageformats"]["apt"] = QStringList();
	ndeps["qtopengl"]["apt"] = QStringList() << "libc6"
											 << "libstdc++6";
	ndeps["qtquickcontrols"]["apt"] = QStringList();
	ndeps["qtquicklayouts"]["apt"] = QStringList();
	ndeps["qtquickdialogs"]["apt"] = QStringList();
	ndeps["qtwebkitexamples"]["apt"] = QStringList();
	ndeps["qtplatformsupport"]["apt"] = QStringList();
	ndeps["qtjsbackend"]["apt"] = QStringList();
	ndeps["qtaccessability"]["apt"] = QStringList();
	return ndeps;
}
}
