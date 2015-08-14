#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <SimpleCrypt/simplecrypt.h>

class Settings {

public:

    static void init();
    static void kill();
    static void reset();

    // Load:

    static QString get_version();
    static QString get_profile();
    static QString get_language();
    static bool get_update();
    static bool get_upload();
    static QString get_last_path();
    static QByteArray get_geometry();
    static QByteArray get_splitter();
    static QString get_temp(bool fallback = true);
    static QStringList get_recent();

    static bool get_use_apktool();
    static int get_compression();
    static bool get_smali();
    static bool get_sign();
    static bool get_zipalign();

    static bool get_use_keystore();
    static QString get_keystore();
    static QString get_keystore_pass();
    static QString get_alias();
    static QString get_alias_pass();
    static QString get_pem();
    static QString get_pk8();

    static bool get_dropbox();
    static bool get_gdrive();
    static bool get_onedrive();
    static QString get_dropbox_token();
    static QString get_gdrive_token();
    static QString get_onedrive_token();

    // Save:

    static void set_version(QString value);
    static void set_profile(QString value);
    static void set_language(QString value);
    static void set_update(bool value);
    static void set_upload(bool value);
    static void set_path(QString value);
    static void set_geometry(QByteArray value);
    static void set_splitter(QByteArray value);
    static void set_recent(QStringList value);
    static void set_temp(QString value);

    static void set_use_apktool(bool value);
    static void set_compression(int value);
    static void set_smali(bool value);
    static void set_sign(bool value);
    static void set_zipalign(bool value);

    static void set_use_keystore(bool value);
    static void set_pem(QString value);
    static void set_pk8(QString value);
    static void set_keystore(QString value);
    static void set_keystore_pass(QString value);
    static void set_alias(QString value);
    static void set_alias_pass(QString value);

    static void set_dropbox(bool value);
    static void set_gdrive(bool value);
    static void set_onedrive(bool value);
    static void set_dropbox_token(QString value);
    static void set_gdrive_token(QString value);
    static void set_onedrive_token(QString value);

protected:

    // Root Section:
    QString s_version;
    QString s_profile;
    QString s_language;
    bool s_update;
    QString s_last_path;
    QByteArray s_window_geometry;
    QByteArray s_window_splitter;
    QStringList s_recent_list;
    QString s_temp_path;

    // "APK" Section:
    bool s_use_apktool;
    int s_compression;
    bool s_smali;
    bool s_sign;
    bool s_zipalign;
    bool s_upload;

    // "Key" Section:
    bool s_use_keystore;
    QString s_pem;
    QString s_pk8;
    QString s_keystore;
    QString s_keystore_pass;
    QString s_alias;
    QString s_alias_pass;

    // Cloud Services Section:
    bool s_dropbox;
    bool s_gdrive;
    bool s_onedrive;
    QString s_dropbox_token;
    QString s_gdrive_token;
    QString s_onedrive_token;

private:

    Settings() {}
    static QSettings *settings;
    static SimpleCrypt *crypt;
    static quint64 key_mac;
};

#endif // SETTINGS_H
