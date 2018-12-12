#include "manifest.h"
#include <QTextStream>
#include <QDebug>

Manifest::Manifest(const QString &xmlPath, const QString &ymlPath)
{
    // XML:

    xmlFile = new QFile(xmlPath);
    if (xmlFile->open(QFile::ReadWrite | QFile::Text)) {
        QTextStream stream(xmlFile);
        stream.setCodec("UTF-8");
        xml.setContent(stream.readAll());

        // Parse <application> node:

        applicationLabel = getXmlAttribute(QStringList() << "application" << "android:label");
        applicationIcon = getXmlAttribute(QStringList() << "application" << "android:icon");
        applicationBanner = getXmlAttribute(QStringList() << "application" << "android:banner");

        // Parse <activity> nodes:

        QDomNodeList nodes = xml.firstChildElement("manifest").firstChildElement("application").childNodes();
        for (int i = 0; i < nodes.count(); ++i) {
            QDomElement node = nodes.at(i).toElement();
            if (node.isElement() && node.nodeName() == "activity") {
                QDomElement activity = node.toElement();
                QDomAttr icon = activity.attributeNode("android:icon");
                if (!icon.isNull()) {
                    activityIcons.append(icon);
                }
                QDomAttr banner = activity.attributeNode("android:banner");
                if (!banner.isNull()) {
                    activityBanners.append(banner);
                }
            }
        }

#ifdef QT_DEBUG
        qDebug() << "Parsed app label:   " << applicationLabel.value();
        qDebug() << "Parsed app icon:    " << applicationIcon.value();
#endif
    }

    // YAML:

    regexMinSdk.setPatternOptions(QRegularExpression::MultilineOption);
    regexTargetSdk.setPatternOptions(QRegularExpression::MultilineOption);
    regexVersionCode.setPatternOptions(QRegularExpression::MultilineOption);
    regexVersionName.setPatternOptions(QRegularExpression::MultilineOption);
    regexMinSdk.setPattern("(?<=^  minSdkVersion: ')\\d+(?='$)");
    regexTargetSdk.setPattern("(?<=^  targetSdkVersion: ')\\d+(?='$)");
    regexVersionCode.setPattern("(?<=^  versionCode: ')\\d+(?='$)");
    regexVersionName.setPattern("(?<=^  versionName: ).+(?=$)");

    ymlFile = new QFile(ymlPath);
    if (ymlFile->open(QFile::ReadWrite | QFile::Text)) {
        QTextStream stream(ymlFile);
        stream.setCodec("UTF-8");
        yml = stream.readAll();
        minSdk = regexMinSdk.match(yml).captured().toInt();
        targetSdk = regexTargetSdk.match(yml).captured().toInt();
        versionCode = regexVersionCode.match(yml).captured().toInt();
        versionName = regexVersionName.match(yml).captured();
#ifdef QT_DEBUG
        qDebug() << "Parsed minumum SDK: " << minSdk;
        qDebug() << "Parsed target SDK:  " << targetSdk;
        qDebug() << "Parsed version code:" << versionCode;
        qDebug() << "Parsed version name:" << versionName;
#endif
    }
}

Manifest::~Manifest()
{
    delete xmlFile;
    delete ymlFile;
}

QDomAttr Manifest::getXmlAttribute(QStringList &tree) const
{
    if (!tree.isEmpty()) {
        const QString attribute = tree.takeLast();
        QDomElement node = xml.firstChildElement("manifest");
        foreach (const QString &element, tree) {
            node = node.firstChildElement(element);
        }
        return node.attributeNode(attribute);
    }
    return QDomAttr();
}

QString Manifest::getApplicationIcon() const
{
    return applicationIcon.value();
}

QString Manifest::getApplicationBanner() const
{
    return applicationBanner.value();
}

QString Manifest::getApplicationLabel() const
{
    return applicationLabel.value();
}

QStringList Manifest::getActivityIcons() const
{
    QStringList values;
    for (int i = 0; i < activityIcons.count(); ++i) {
        const QString value = activityIcons.at(i).value();
        if (value != getApplicationIcon()) {
            values.append(value);
        }
    }
    values.removeDuplicates();
    return values;
}

QStringList Manifest::getActivityBanners() const
{
    QStringList values;
    for (int i = 0; i < activityBanners.count(); ++i) {
        const QString value = activityBanners.at(i).value();
        if (value != getApplicationBanner()) {
            values.append(value);
        }
    }
    values.removeDuplicates();
    return values;
}

int Manifest::getMinSdk() const
{
    return minSdk;
}

int Manifest::getTargetSdk() const
{
    return targetSdk;
}

int Manifest::getVersionCode() const
{
    return versionCode;
}

QString Manifest::getVersionName() const
{
    return versionName;
}

void Manifest::setApplicationIcon(const QString &value)
{
    applicationIcon.setValue(value);
    saveXml();
}

void Manifest::setApplicationLabel(const QString &value)
{
    applicationLabel.setValue(value);
    saveXml();
}

void Manifest::setMinSdk(int value)
{
    value = qMax(0, value);
    minSdk = value;
    yml.replace(regexMinSdk, QString::number(value));
    saveYml();
}

void Manifest::setTargetSdk(int value)
{
    value = qMax(1, value);
    targetSdk = value;
    yml.replace(regexTargetSdk, QString::number(value));
    saveYml();
}

void Manifest::setVersionCode(int value)
{
    value = qMax(0, value);
    versionCode = value;
    yml.replace(regexVersionCode, QString::number(value));
    saveYml();
}

void Manifest::setVersionName(const QString &value)
{
    versionName = value;
    yml.replace(regexVersionName, value);
    saveYml();
}

bool Manifest::addApplicationBanner()
{
    // TODO Refactor:

    QDomElement application = xml.firstChildElement("manifest").firstChildElement("application");
    QDomElement intent;

    QDomElement activity = application.firstChildElement("activity");
    while (!activity.isNull()) {
        intent = findIntentByCategory(activity, "LAUNCHER");
        if (!intent.isNull()) {
            break;
        }
        activity = activity.nextSiblingElement();
    }

    // If the "activity" tag was not found, try the "activity-alias" tag:

    if (intent.isNull()) {
        QDomElement activity = application.firstChildElement("activity-alias");
        while (!activity.isNull()) {
            intent = findIntentByCategory(activity, "LAUNCHER");
            if (!intent.isNull()) {
                break;
            }
            activity = activity.nextSiblingElement();
        }
    }

    if (!intent.isNull()) {
        // Add intent:
        QDomElement tvlauncher = xml.createElement("category");
        tvlauncher.setTagName("category");
        tvlauncher.setAttribute("android:name", "android.intent.category.LEANBACK_LAUNCHER");
        intent.appendChild(tvlauncher);
        // Add application attribute:
        application.setAttribute("android:banner", "@drawable/banner_custom");
        applicationBanner = getXmlAttribute(QStringList() << "application" << "android:banner");
        return saveXml();
    }
    return true;
}

bool Manifest::saveXml()
{
    if (xmlFile->isWritable()) {
        xmlFile->resize(0);
        QTextStream stream(xmlFile);
        stream.setCodec("UTF-8");
        xml.save(stream, 4);
        return true;
    } else {
        qWarning() << "Error: Could not save AndroidManifest.xml";
        return false;
    }
}

bool Manifest::saveYml()
{
    if (ymlFile->isWritable()) {
        ymlFile->resize(0);
        QTextStream stream(ymlFile);
        stream.setCodec("UTF-8");
        stream << yml;
        return true;
    } else {
        qWarning() << "Error: Could not save apktool.yml";
        return false;
    }
}

QDomElement Manifest::findIntentByCategory(QDomElement activity, QString category)
{
    QDomElement intent = activity.firstChildElement("intent-filter");
    while (!intent.isNull()) {
        QDomElement cat = intent.firstChildElement("category");
        while (!cat.isNull()) {
            if (cat.attribute("android:name") == "android.intent.category." + category) {
                return intent;
            }
            cat = cat.nextSiblingElement();
        }
        intent = intent.nextSiblingElement();
    }
    return QDomElement();
}
