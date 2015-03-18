#ifndef TEST_APKTOOL_H
#define TEST_APKTOOL_H

#include <QObject>

// TestApktool

class TestApktool : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void test();
};

// Apktool

class Apktool {

public:
    static bool unpack(const QString FILENAME);
    static bool get_icons(const QString MANIFEST, bool critical);
    static bool is_java_installed();

private:
    static QString read_manifest(const QString FILENAME);
    static QString parse(QString REGEXP, QString STR);
};

#endif // TEST_APKTOOL_H
