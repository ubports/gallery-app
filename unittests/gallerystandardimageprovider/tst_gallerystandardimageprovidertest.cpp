#include <QString>
#include <QtTest>

class GallerystandardimageproviderTest : public QObject
{
    Q_OBJECT
    
public:
    GallerystandardimageproviderTest();
    
private Q_SLOTS:
    void testCase1();
};

GallerystandardimageproviderTest::GallerystandardimageproviderTest()
{
}

void GallerystandardimageproviderTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(GallerystandardimageproviderTest)

#include "tst_gallerystandardimageprovidertest.moc"
