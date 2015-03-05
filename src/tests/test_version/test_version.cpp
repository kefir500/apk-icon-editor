#include "../../updater.h"
#include <QtTest>

class TestVersion : public QObject {
    Q_OBJECT

public:
    TestVersion() { }

private Q_SLOTS:
    void test();
};

void TestVersion::test()
{
    Updater updater;

    // Objective: ARG1 > ARG2
    QVERIFY(updater.compare("1.1", "1.0"));
    QVERIFY(updater.compare("1.1.0", "1.0"));
    QVERIFY(updater.compare("1.0.1", "1.0"));
    QVERIFY(updater.compare("1.0.0.19", "1.0"));
    QVERIFY(updater.compare("1.0.0.19", "1.0.0.2"));
    QVERIFY(updater.compare("1.0.0.19", "1.0.0.2.777"));

    // Objective: ARG1 == ARG2
    QCOMPARE(updater.compare("1.0", "1.0.0.0"), false);
}

QTEST_MAIN(TestVersion)

#include "test_version.moc"
