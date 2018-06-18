///
/// \file
/// This file contains the application settings interface.
///

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <SimpleCrypt/simplecrypt.h>

///
/// Settings wrapper.
/// This class represents a static interface for reading and writing application settings.
///

class Settings {

public:

    static bool init();  ///< Initializes settings interface.
    static void kill();  ///< Frees settings interface.
    static void reset(); ///< Resets applciation settings to default values.

    // Load:

    static QString get_version();        ///< Returns the application version at the time of the previous launch.
    static QString get_device();         ///< Returns the last used device preset.
    static QString get_language();       ///< Returns the last used application language.
    static bool get_update();            ///< Returns \c true if the automatic update check is turned on.
    static bool get_upload();            ///< Returns \c true if the cloud upload is turned on.
    static QString get_last_path();      ///< Returns the last used directory in the open/save dialog.
    static QByteArray get_geometry();    ///< Returns the position, size and state of the main window.
    static QByteArray get_splitter();    ///< Returns the position of the main window splitter.
    static QStringList get_recent();     ///< Returns the list of the recent APK files.

    /// Returns the path to Apktool.
    /// \param fallback If \c true, the default value will be returned instead of *nonexistent* Apktool path.
    static QString get_apktool(bool fallback = true);
    static bool get_use_apksigner();     ///< Returns \c true if the apksigner is used instead of signapk / jarsigner.
    static bool get_smali();             ///< Returns \c true if smali/baksmali tool is used.
    static bool get_sign();              ///< Returns \c true if the signing of the APK files is turned on.
    static bool get_zipalign();          ///< Returns \c true if the ZIP-aligning (optimizing) APK files is turned on.

    static bool get_use_keystore();      ///< Returns \c true if the KeyStore is used instead of PEM/PK8.
    static QString get_keystore();       ///< Returns the path to the KeyStore file.
    static QString get_keystore_pass();  ///< Returns the decrypted keyStore password.
    static QString get_alias();          ///< Returns the value of the KeyStore alias.
    static QString get_alias_pass();     ///< Returns the decrypted KeyStore alias password.
    static QString get_pem();            ///< Returns the path to the PEM file.
    static QString get_pk8();            ///< Returns the path to the PK8 file.

    static bool get_dropbox();           ///< Returns \c true if Dropbox upload is turned on.
    static bool get_gdrive();            ///< Returns \c true if Google Drive upload is turned on.
    static bool get_onedrive();          ///< Returns \c true if Microsoft OneDrive upload is turned on.
    static QString get_dropbox_token();  ///< Returns the decrypted Dropbox secret token.
    static QString get_gdrive_token();   ///< Returns the decrypted Google Drive secret token.
    static QString get_onedrive_token(); ///< Returns the decrypted Microsoft OneDrive secret token.

    /// Returns the temporary directory path.
    /// \param fallback If \c true, the default system value will be returned instead of *nonexistent* temporary path.
    static QString get_temp(bool fallback = true);

    // Save:

    static void set_version(QString version);        ///< Saves the current application \c version.
    static void set_device(QString device);          ///< Saves the last used \c device preset.
    static void set_language(QString language);      ///< Saves the last used application \c language.
    static void set_update(bool state);              ///< Saves the automatic update check on/off \c state.
    static void set_upload(bool state);              ///< Saves the cloud upload on/off \c state.
    static void set_path(QString directory);         ///< Saves the open/save dialog \c directory.
    static void set_geometry(QByteArray value);      ///< Saves the main window position, size and state as \c value.
    static void set_splitter(QByteArray value);      ///< Saves the main window splitter position \c value.
    static void set_recent(QStringList recent);      ///< Saves the \c recent file list.
    static void set_temp(QString path);              ///< Saves the temporary directory \c path.

    static void set_apktool(QString path);           ///< Saves the \c path to Apktool.
    static void set_use_apksigner(bool state);       ///< Saves the Apksigner on/off \c state.
    static void set_smali(bool state);               ///< Saves the smali/baksmali on/off \c state.
    static void set_sign(bool state);                ///< Saves the APK signing on/off \c state.
    static void set_zipalign(bool state);            ///< Saves the APK ZIP-aligning (optimizing) on/off \c state.

    static void set_use_keystore(bool state);        ///< Saves the KeyStore on/off \c state.
    static void set_pem(QString path);               ///< Saves the \c path to the PEM file.
    static void set_pk8(QString path);               ///< Saves the \c path to the PK8 file.
    static void set_keystore(QString path);          ///< Saves the \c path to the KeyStore file.
    static void set_keystore_pass(QString password); ///< Saves the encrypted KeyStore \c password.
    static void set_alias(QString value);            ///< Saves the KeyStore alias \c value.
    static void set_alias_pass(QString password);    ///< Saves the encrypted KeyStore alias \c password.

    static void set_dropbox(bool state);             ///< Saves the Dropbox upload on/off \c state.
    static void set_gdrive(bool state);              ///< Saves the Google Drive upload on/off \c state.
    static void set_onedrive(bool state);            ///< Saves the Microsoft OneDrive upload on/off \c state.
    static void set_dropbox_token(QString token);    ///< Saves the encrypted Dropbox secret \c token.
    static void set_gdrive_token(QString token);     ///< Saves the encrypted Google Drive secret \c token.
    static void set_onedrive_token(QString token);   ///< Saves the encrypted Microsoft OneDrive secret \c token.

private:

    Settings() {}
    static QSettings *settings; ///< Settings object.
    static SimpleCrypt *crypt;  ///< Password encryptor/decryptor.
    static quint64 key_mac;     ///< Cryptographic key based on the network interface MAC address.
};

#endif // SETTINGS_H
