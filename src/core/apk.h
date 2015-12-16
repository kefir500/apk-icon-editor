/// \file apk.h
/// \brief This file contains #Apk class declaration.

#ifndef PACKER_H
#define PACKER_H

#include "icon.h"
#include "profile.h" // for Dpi enum

class Resource {
private:
    QString filename;
    QString value;
public:
    QString getFilename() const { return filename; }
    QString getValue() const { return value; }
    Resource(QString _filename, QString _value)
        : filename(_filename), value(_value) { }
};

struct PackOptions {
    QString filename;           ///< Output APK filename.
    QString temp;               ///< Directory for temporary files.
    bool isApktool;             ///< If \c TRUE, use "apktool". If \c FALSE, use "7za".
    short ratio;                ///< 7za compression ratio [0-9] (only in "7za" mode).
    bool isSmali;               ///< If \c TRUE, decompile "classes.dex" (only in "apktool" mode).
    bool isSign;                ///< If \c TRUE, sign APK.
    bool isOptimize;            ///< If \c TRUE, optimize (zipalign) APK.
    bool isKeystore;            ///< If \c TRUE, use "KeyStore". If \c FALSE, use "PEM/PK8".
    QString filePem;            ///< Path to PEM file.
    QString filePk8;            ///< Path to PK8 file.
    QString fileKey;            ///< Path to KeyStore file.
    QString alias;              ///< KeyStore alias.
    QString passStore;          ///< KeyStore password.
    QString passAlias;          ///< KeyStore alias password.
    QString appName;            ///< Output APK name (only in "apktool" mode).
    QString appVersionName;     ///< Output APK version name (only in "apktool" mode).
    QString appVersionCode;     ///< Output APK version code (only in "apktool" mode).
    QList<Resource> resources;  ///< List of edited (string) resources.
};

/// \brief APK manager class.
///
/// Class for performing operations on APK files: #pack, #unpack, #sign and #optimize.<br>
/// Internal processes run in the separate #thread.
///
/// \image html apk.png

class Apk : public QObject {
    Q_OBJECT

private:
    QString filename;                       ///< Current APK filename.
    QString temp;                           ///< Temporary directory for repacking.
    QString manifest;                       ///< Stores AndroidManifest.xml.
    QString var_androidLabel;               ///< Stores the name of "android:label" variable (AndroidManifest.xml).
    QString warnText;                       ///< Stores warning text.
    QList<QSharedPointer<Icon> > icons;     ///< Stores [shared pointers to] loaded APK icons.
    QList<Resource> strings;                ///< Stores APK string resources.

    static const char *STR_ERROR;           ///< Error display text template.
    static const char *STR_ERRORSTART;      ///< Error display text template.
    static const char *STR_CHECKPATH;       ///< "Check the PATH variable" string.
    static const QString LOG_ERRORSTART;    ///< Error log text template.
    static const QString LOG_EXITCODE;      ///< Error log text template.

    /// \brief This function has to be called on fatal error.
    /// Emits #error signal and returns \c FALSE.
    /// \param[in] title Error message brief title.
    /// \param[in] text  Error message detailed text.
    /// \return This function always returns \c FALSE.
    bool die(QString title, QString text) const;

    /// \brief Checks if 7za worked successfully by its exit code.
    /// \param[in] code 7za exit code.
    /// \retval \c TRUE if 7za succesfully zipped/unzipped APK (#code equals 0).
    /// \retval \c FALSE if 7za did not finish successfully (#code is non-zero).
    bool getZipSuccess(int code) const;

    /// \brief Start immediate APK unpacking.
    /// \return \c TRUE on success.
    /// \warning This function MUST emit #error or #unpacked signal before exit.
    bool doUnpack(PackOptions options);

    /// \brief Start immediate APK packing.
    /// \return \c TRUE on success.
    /// \warning This function MUST emit #error or #packed signal before exit.
    bool doPack(PackOptions opts);

    // Unpacking APK:
    bool readManifest();                    ///< Read AndroidManifest.xml and save it to #manifest variable.
    bool unzip() const;                     ///< Unpack APK to temporary directory using "7za" tool.
    bool unzip_apktool(bool smali) const;   ///< Unpack APK to temporary directory using "apktool" tool. If \c smali is \c TRUE, decompile source.
    void loadIcons();                       ///< Load APK icons.
    void loadStrings();                     ///< Load APK string resources (only in "apktool" mode).

    // Packing APK:
    /// Save edited XML resources.
    void saveXmlChanges(QString appName,
                        QString versionName,
                        QString versionCode,
                        QList<Resource> resources) const;
    bool saveIcons() const;             ///< Save icons from #icons list to PNG images.
    bool zip(short ratio = 9) const;    ///< Pack APK with specified compression \c ratio using "7za" tool.
    bool zip_apktool() const;           ///< Pack APK using "apktool" tool.

    /// Sign APK using "signapk" tool.
    bool sign(const QString PEM, const QString PK8) const;

    /// Sign APK using "jarsigner" tool.
    bool sign(const QString KEY, const QString ALIAS,
              const QString PASS_STORE, const QString PASS_KEY) const;

    bool optimize() const;              ///< Optimize APK using "zipalign" tool.
    bool finalize();                    ///< Move the final APK from temporary to user-specified directory.

    enum JavaType { JRE, JDK };

public:
    /// \brief Get APK icon from #icons list.
    /// \param[in] id DPI of the required icon.
    /// \return Requested #icon.
    Icon *getIcon(Dpi id) const;

    QList<Resource> getStrings() const;     ///< Returns APK string resource list.
    QString getPackageName() const;         ///< Returns APK package name.
    QString getVersionCode() const;         ///< Returns APK version code.
    QString getVersionName() const;         ///< Returns APK display version.
    QString getApplicationLabel() const;    ///< Returns APK display name.
    QString getMinimumSdk() const;          ///< Returns APK minimum SDK version.
    QString getTargetSdk() const;           ///< Returns APK target SDK version.

    static bool isJavaInstalled();                ///< Returns \c TRUE if Java (JRE or JDK) is installed.
    static QString getJavaVersion(JavaType java); ///< Returns Java version.
    static QString getJreVersion();               ///< Returns JRE version.
    static QString getJdkVersion();               ///< Returns JDK version.
    static QString getApktoolVersion();           ///< Returns Apktool version.

    void unpack(PackOptions options);       ///< Unpack APK by calling private #doUnpack method in a separate #thread.
    void pack(PackOptions options);         ///< Pack APK by calling private #doPack method in a separate #thread.

    explicit Apk(QObject *parent) : QObject(parent) { }

signals:
    /// \brief This signal represents current loading state.
    /// \param[in] percentage Loading percentage (0-100).
    /// \param[in] text       Loading text.
    void loading(short percentage, QString text = QString()) const;

    /// \brief This signal is emitted to send error message.
    /// \param[in] title Message brief title.
    /// \param[in] text  Message detailed text.
    void error(QString title, QString text) const;

    /// \brief This signal is emitted to send warning message.
    /// \param[in] title Message brief title.
    /// \param[in] text  Message detailed text.
    void warning(QString title, QString text) const;

    /// \brief This signal is emitted when APK is successfully unpacked.
    /// \param[in] filename Filename (with full path) of just unpacked APK.
    void unpacked(QString filename) const;

    /// \brief This signal is emitted when APK is successfully packed.
    /// \param[in] filename Filename (with full path) of just packed APK.
    /// \param[in] success  \c FALSE if APK is packed with warnings.
    /// \param[in] text     Additional message text.
    void packed(QString filename, bool success = true, QString text = QString()) const;

public slots:
    /// \brief Delete temporary files.
    void clearTemp();
};

#endif // PACKER_H
