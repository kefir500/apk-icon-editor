#ifndef MANIFEST_H
#define MANIFEST_H

#include <QFile>
#include <QDomDocument>
#include <QRegularExpression>

class Manifest {

public:
    Manifest(const QString &xmlPath, const QString &ymlPath);
    ~Manifest();

    QDomAttr getXmlAttribute(QStringList &tree) const;

    QString getApplicationIcon() const;
    QString getApplicationBanner() const;
    QString getApplicationLabel() const;
    QStringList getActivityIcons() const;
    QStringList getActivityBanners() const;
    int getMinSdk() const;
    int getTargetSdk() const;
    int getVersionCode() const;
    QString getVersionName() const;

    void setApplicationIcon(const QString &value);
    void setApplicationLabel(const QString &value);
    void setMinSdk(int value);
    void setTargetSdk(int value);
    void setVersionCode(int value);
    void setVersionName(const QString &value);

    bool addApplicationIcon();
    bool addApplicationBanner();

private:
    bool saveXml();
    bool saveYml();

    QDomElement findIntentByCategory(QDomElement activity, QString category);

    QFile *xmlFile;
    QFile *ymlFile;

    QDomDocument xml;
    QString yml;

    QDomAttr applicationIcon;
    QDomAttr applicationBanner;
    QDomAttr applicationLabel;
    QList<QDomAttr> activityIcons;
    QList<QDomAttr> activityBanners;

    int minSdk;
    int targetSdk;
    int versionCode;
    QString versionName;

    QRegularExpression regexMinSdk;
    QRegularExpression regexTargetSdk;
    QRegularExpression regexVersionCode;
    QRegularExpression regexVersionName;
};

#endif // MANIFEST_H
