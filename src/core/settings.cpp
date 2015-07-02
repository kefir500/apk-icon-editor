#include "settings.h"
#include "keys.h"
#include <QDir>
#include <QNetworkInterface>
#include <QApplication>

#define APPDIR QCoreApplication::applicationDirPath()

QSettings *Settings::settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "apk-icon-editor", "config");
SimpleCrypt *Settings::crypt = new SimpleCrypt();
quint64 Settings::key_mac = 0;

void Settings::init()
{
    // Init encryption / decryption:

    QString strKeyMac;
    foreach (QNetworkInterface net, QNetworkInterface::allInterfaces()) {
        if (!(net.flags() & QNetworkInterface::IsLoopBack)) {
            QStringList mac = net.hardwareAddress().split(':');
            if (mac.size() >= 6) {
                strKeyMac = "0x" + mac[0] + mac[1] + mac[2]
                                 + mac[3] + mac[4] + mac[5]
                                 + mac[0] + mac[1];
                break;
            }
        }
    }

    bool ok;
    key_mac = strKeyMac.toULongLong(&ok, 16);
    if (!ok) { key_mac = SIMPLECRYPT_KEY; }
}

void Settings::kill()
{
    delete settings;
    delete crypt;
}

void Settings::reset()
{
    settings->clear();
}

// Load:

QString Settings::get_version()     { return settings->value("Version", "").toString(); }
QString Settings::get_profile()     { return settings->value("Profile", "").toString(); }
QString Settings::get_language()    { return settings->value("Language", QLocale::system().name()).toString(); }
bool Settings::get_update()         { return settings->value("Update", true).toBool(); }
bool Settings::get_upload()         { return settings->value("Upload", false).toBool(); }
QString Settings::get_last_path()   { return settings->value("Directory", "").toString(); }
QByteArray Settings::get_geometry() { return settings->value("Geometry", 0).toByteArray(); }
QByteArray Settings::get_splitter() { return settings->value("Splitter", 0).toByteArray(); }
QStringList Settings::get_recent()  { return settings->value("Recent", 0).toStringList(); }

QString Settings::get_temp(bool fallback)
{
    const QString TEMP = QDir::toNativeSeparators(QDir::tempPath());
    const QString PATH = settings->value("Temp", TEMP).toString();
    return (fallback && (!QDir(PATH).exists() || PATH.isEmpty())) ? TEMP : PATH;
}

bool Settings::get_use_apktool() { return settings->value("APK/Apktool", false).toBool(); }
int Settings::get_compression()  { return settings->value("APK/Compression", 9).toInt(); }
bool Settings::get_smali()       { return settings->value("APK/Smali", false).toBool(); }
bool Settings::get_sign()        { return settings->value("APK/Sign", true).toBool(); }
bool Settings::get_zipalign()    { return settings->value("APK/Optimize", true).toBool(); }

bool Settings::get_use_keystore()     { return settings->value("Key/Method", false).toBool(); }
QString Settings::get_keystore()      { return settings->value("Key/KeyStore", "").toString(); }
QString Settings::get_alias()         { return settings->value("Key/Alias", "").toString(); }

QString Settings::get_pem()
{
    const QString PEM = settings->value("Key/PEM", "").toString();
    return QFile::exists(PEM) ? PEM : APPDIR + "/signer/certificate.pem";
}

QString Settings::get_pk8()
{
    const QString PK8 = settings->value("Key/PK8", "").toString();
    return QFile::exists(PK8) ? PK8 : APPDIR + "/signer/key.pk8";
}

QString Settings::get_keystore_pass()
{
    QString password = settings->value("Key/PassStore", "").toString();
    crypt->setKey(SIMPLECRYPT_KEY);
    password = crypt->decryptToString(password);
    crypt->setKey(key_mac);
    password = crypt->decryptToString(password);
    return password;
}

QString Settings::get_alias_pass()
{
    QString password = settings->value("Key/PassAlias", "").toString();
    crypt->setKey(SIMPLECRYPT_KEY);
    password = crypt->decryptToString(password);
    crypt->setKey(key_mac);
    password = crypt->decryptToString(password);
    return password;
}

bool Settings::get_dropbox()           { return settings->value("Dropbox/Enable", false).toBool(); }
bool Settings::get_gdrive()            { return settings->value("GDrive/Enable", false).toBool(); }
bool Settings::get_onedrive()          { return settings->value("OneDrive/Enable", false).toBool(); }
QString Settings::get_dropbox_token()  { return settings->value("Dropbox/Token", "").toString(); }
QString Settings::get_gdrive_token()   { return settings->value("GDrive/Token", "").toString(); }
QString Settings::get_onedrive_token() { return settings->value("OneDrive/Token", "").toString(); }

// Save:

void Settings::set_version(QString value)     { settings->setValue("Version", value); }
void Settings::set_profile(QString value)     { settings->setValue("Profile", value); }
void Settings::set_language(QString value)    { settings->setValue("Language", value); }
void Settings::set_update(bool value)         { settings->setValue("Update", value); }
void Settings::set_upload(bool value)         { settings->setValue("Upload", value); }
void Settings::set_path(QString value)        { settings->setValue("Directory", value); }
void Settings::set_geometry(QByteArray value) { settings->setValue("Geometry", value); }
void Settings::set_splitter(QByteArray value) { settings->setValue("Splitter", value); }
void Settings::set_recent(QStringList value)  { settings->setValue("Recent", value); }
void Settings::set_temp(QString value)        { settings->setValue("Temp", value); }

void Settings::set_use_apktool(bool value) { settings->setValue("APK/Apktool", value); }
void Settings::set_compression(int value)  { settings->setValue("APK/Compression", value); }
void Settings::set_smali(bool value)       { settings->setValue("APK/Smali", value); }
void Settings::set_sign(bool value)        { settings->setValue("APK/Sign", value); }
void Settings::set_zipalign(bool value)    { settings->setValue("APK/Optimize", value); }

void Settings::set_use_keystore(bool value)     { settings->setValue("Key/Method", value); }
void Settings::set_keystore(QString value)      { settings->setValue("Key/KeyStore", value); }
void Settings::set_alias(QString value)         { settings->setValue("Key/Alias", value); }
void Settings::set_pem(QString value)           { settings->setValue("Key/PEM", value); }
void Settings::set_pk8(QString value)           { settings->setValue("Key/PK8", value); }

void Settings::set_keystore_pass(QString value)
{
    QString password = value;
    crypt->setKey(key_mac);
    password = crypt->encryptToString(password);
    crypt->setKey(SIMPLECRYPT_KEY);
    password = crypt->encryptToString(password);
    settings->setValue("Key/PassStore", password);
}

void Settings::set_alias_pass(QString value)
{
    QString password = value;
    crypt->setKey(key_mac);
    password = crypt->encryptToString(password);
    crypt->setKey(SIMPLECRYPT_KEY);
    password = crypt->encryptToString(password);
    settings->setValue("Key/PassAlias", password);
}

void Settings::set_dropbox(bool value)           { settings->setValue("Dropbox/Enable", value); }
void Settings::set_gdrive(bool value)            { settings->setValue("GDrive/Enable", value); }
void Settings::set_onedrive(bool value)          { settings->setValue("OneDrive/Enable", value); }
void Settings::set_dropbox_token(QString value)  { settings->setValue("Dropbox/Token", value); }
void Settings::set_gdrive_token(QString value)   { settings->setValue("GDrive/Token", value); }
void Settings::set_onedrive_token(QString value) { settings->setValue("OneDrive/Token", value); }
