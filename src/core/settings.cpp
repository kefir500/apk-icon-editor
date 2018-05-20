#include "settings.h"
#include "globals.h"
#include "keys.h"
#include <QDir>
#include <QNetworkInterface>
#include <QApplication>

QSettings *Settings::settings = NULL;
SimpleCrypt *Settings::crypt = NULL;
quint64 Settings::key_mac = 0;

bool Settings::init()
{
    if (settings) {
        return false;
    }

#ifndef PORTABLE
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "apk-icon-editor", "config");
#else
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, Path::App::dir() + "data");
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "settings", "config");
#endif

    crypt = new SimpleCrypt();

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

    return true;
}

void Settings::kill()
{
    if (settings) delete settings;
    if (crypt) delete crypt;
}

void Settings::reset()
{
    if (settings) settings->clear();
}

// Load:

QString Settings::get_version()     { return settings->value("Version", "").toString(); }
QString Settings::get_device()      { return settings->value("Profile", "").toString(); }
QString Settings::get_language()    { return settings->value("Language", QLocale::system().name()).toString(); }
bool Settings::get_update()         { return settings->value("Update", true).toBool(); }
bool Settings::get_upload()         { return settings->value("Upload", false).toBool(); }
QString Settings::get_last_path()   { return settings->value("Directory", "").toString(); }
QByteArray Settings::get_geometry() { return settings->value("Geometry", 0).toByteArray(); }
QByteArray Settings::get_splitter() { return settings->value("Splitter", 0).toByteArray(); }
QStringList Settings::get_recent()  { return settings->value("Recent", 0).toStringList(); }

QString Settings::get_temp(bool fallback)
{
    const QString TEMP = QDir::toNativeSeparators(Path::Data::temp());
    const QString PATH = settings->value("Temp", TEMP).toString();
    return (fallback && (!QDir(PATH).exists() || PATH.isEmpty())) ? TEMP : PATH;
}

QString Settings::get_apktool()
{
    const QString APKTOOL = QDir::toNativeSeparators(Path::Data::shared() + "apktool.jar");
    return settings->value("Paths/Apktool", APKTOOL).toString();
}

bool Settings::get_use_apksigner() { return settings->value("APK/Apksigner", false).toBool(); }
bool Settings::get_smali()         { return settings->value("APK/Smali", false).toBool(); }
bool Settings::get_sign()          { return settings->value("APK/Sign", true).toBool(); }
bool Settings::get_zipalign()      { return settings->value("APK/Optimize", true).toBool(); }

bool Settings::get_use_keystore() { return settings->value("Key/Method", false).toBool(); }
QString Settings::get_keystore()  { return settings->value("Key/KeyStore", "").toString(); }
QString Settings::get_alias()     { return settings->value("Key/Alias", "").toString(); }

QString Settings::get_pem()
{
    const QString PEM = settings->value("Key/PEM", "").toString();
    return QFile::exists(PEM) ? PEM : Path::Data::shared() + "signer/certificate.pem";
}

QString Settings::get_pk8()
{
    const QString PK8 = settings->value("Key/PK8", "").toString();
    return QFile::exists(PK8) ? PK8 : Path::Data::shared() + "signer/key.pk8";
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

void Settings::set_version(QString version)   { settings->setValue("Version", version); }
void Settings::set_device(QString device)     { settings->setValue("Profile", device); }
void Settings::set_language(QString language) { settings->setValue("Language", language); }
void Settings::set_update(bool state)         { settings->setValue("Update", state); }
void Settings::set_upload(bool state)         { settings->setValue("Upload", state); }
void Settings::set_path(QString directory)    { settings->setValue("Directory", directory); }
void Settings::set_geometry(QByteArray value) { settings->setValue("Geometry", value); }
void Settings::set_splitter(QByteArray value) { settings->setValue("Splitter", value); }
void Settings::set_recent(QStringList recent) { settings->setValue("Recent", recent); }
void Settings::set_temp(QString path)         { settings->setValue("Temp", path); }

void Settings::set_apktool(QString path)      { settings->setValue("Paths/Apktool", path); }
void Settings::set_use_apksigner(bool state)  { settings->setValue("APK/Apksigner", state); }
void Settings::set_smali(bool state)          { settings->setValue("APK/Smali", state); }
void Settings::set_sign(bool state)           { settings->setValue("APK/Sign", state); }
void Settings::set_zipalign(bool state)       { settings->setValue("APK/Optimize", state); }

void Settings::set_use_keystore(bool state)   { settings->setValue("Key/Method", state); }
void Settings::set_keystore(QString path)     { settings->setValue("Key/KeyStore", path); }
void Settings::set_alias(QString value)       { settings->setValue("Key/Alias", value); }
void Settings::set_pem(QString path)          { settings->setValue("Key/PEM", path); }
void Settings::set_pk8(QString path)          { settings->setValue("Key/PK8", path); }

void Settings::set_keystore_pass(QString password)
{
    crypt->setKey(key_mac);
    password = crypt->encryptToString(password);
    crypt->setKey(SIMPLECRYPT_KEY);
    password = crypt->encryptToString(password);
    settings->setValue("Key/PassStore", password);
}

void Settings::set_alias_pass(QString password)
{
    crypt->setKey(key_mac);
    password = crypt->encryptToString(password);
    crypt->setKey(SIMPLECRYPT_KEY);
    password = crypt->encryptToString(password);
    settings->setValue("Key/PassAlias", password);
}

void Settings::set_dropbox(bool state)           { settings->setValue("Dropbox/Enable", state); }
void Settings::set_gdrive(bool state)            { settings->setValue("GDrive/Enable", state); }
void Settings::set_onedrive(bool state)          { settings->setValue("OneDrive/Enable", state); }
void Settings::set_dropbox_token(QString token)  { settings->setValue("Dropbox/Token", token); }
void Settings::set_gdrive_token(QString token)   { settings->setValue("GDrive/Token", token); }
void Settings::set_onedrive_token(QString token) { settings->setValue("OneDrive/Token", token); }
