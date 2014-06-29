#include <QTest>

#include "testutil.h"

#include "util_core.h"

class Test_UtilCore : public QObject
{
	Q_OBJECT
private slots:
	void isVersionHigherThan_data()
	{
		QTest::addColumn<bool>("result");
		QTest::addColumn<QString>("higher");
		QTest::addColumn<QString>("lower");
		QTest::addColumn<bool>("developerMode");

		QTest::newRow("one number each, true") << true << "5" << "2" << false;
		QTest::newRow("one number each, false") << false << "2" << "5" << false;
		QTest::newRow("one resp. four numbers, true") << true << "5" << "4.9.9.9" << false;
		QTest::newRow("one resp. four numbers, false") << false << "5" << "5.0.0.1" << false;
		QTest::newRow("four resp. one number, true") << true << "5.0.0.1" << "5" << false;
		QTest::newRow("four resp. one number, false") << false << "4.9.9.9" << "5" << false;
		QTest::newRow("four numbers each, true") << true << "10.0.12.0" << "10.0.11.0" << false;
		QTest::newRow("four numbers each, false") << false << "10.0.12.0" << "10.0.13.0" << false;
		QTest::newRow("developer mode, alpha resp. stable, true") << true << "5.3alpha" << "5.2" << true;
		// FIXME: this fails
		//QTest::newRow("not developer mode, alpha resp. stable, false") << false << "5.3alpha" << "5.2" << false;
	}

	void isVersionHigherThan()
	{
		QFETCH(bool, result);
		QFETCH(QString, higher);
		QFETCH(QString, lower);
		QFETCH(bool, developerMode);

		QCOMPARE(Util::isVersionHigherThan(higher, lower, developerMode), result);
	}
};

QTEST_APPLESS_MAIN(Test_UtilCore)

#include "tst_util_core.moc"
