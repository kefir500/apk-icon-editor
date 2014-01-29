/// \file apk.h
/// \brief This file contains #Apk class declaration.

#ifndef PACKER_H
#define PACKER_H

#include "icon.h"
#include "profile.h" // for Dpi enum

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
    QString manifest;                       ///< Stores AndroidManifest.xml.
    QList<QSharedPointer<Icon>> icons;      ///< Stores [shared pointers to] loaded APK icons.
    static const QString STR_ERROR;         ///< Error text template.
    static const QString STR_ERRORSTART;    ///< Error text template.
    static const QString TEMPDIR_APK;       ///< Temporary directory for unpacked resources.

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
    /// \todo Write a wrapper function to improve design.
    bool doUnpack();

    /// \brief Start immediate APK packing.
    /// \param[in] ratio 7za compression ratio (0-9).
    /// \param[in] doSign If \c TRUE, sign APK.
    /// \param[in] doOptimize If \c TRUE, optimize APK.
    /// \return \c TRUE on success.
    /// \warning This function MUST emit #error or #packed signal before exit.
    /// \todo Write a wrapper function to improve design.
    bool doPack(short ratio = 9, bool doSign = true, bool doOptimize = true);

    // Unpacking APK:
    bool readManifest();    ///< Read AndroidManifest.xml and save it to #manifest variable.
    bool unzip() const;     ///< Unpack APK to temporary directory using "7za" tool.
    void loadIcons();       ///< Load APK icons.

    // Packing APK:
    bool saveIcons() const;             ///< Save icons from #icons list to PNG images.
    bool zip(short ratio = 9) const;    ///< Pack APK  with specified compression \c ratio uusing "7za" tool.
    bool checkJavaInstalled() const;    ///< Check if Java Runtime Environment is installed.
    bool sign() const;                  ///< Sign APK using "signapk" tool.
    bool optimize() const;              ///< Optimize APK using "zipalign" tool.
    bool finalize();                    ///< Move the final APK from temporary to user-specified directory.

public:
    /// \brief Get APK icon from #icons list.
    /// \param[in] id DPI of the required icon.
    /// \return Requested #icon.
    Icon *getIcon(Dpi id) const;

    /// \brief Unpack APK by calling private #doUnpack method in a separate #thread.
    /// \param[in] _filename Name of APK file to unpack.
    void unpack(QString filename);

    /// \brief Pack APK by calling private #doPack method in a separate #thread.
    /// \param[in] _filename Name of APK file to pack.
    /// \param[in] ratio Compression ratio (0-9).
    /// \param[in] doSign If \c TRUE, sign APK.
    /// \param[in] doOptimize If \c TRUE, optimize APK.
    void pack(QString filename, short ratio, bool doSign, bool doOptimize);

    explicit Apk(QObject *parent) : QObject(parent) { }

signals:
    /// \brief This signal represents current loading state.
    /// \param[in] percentage Loading percentage (0-100).
    /// \param[in] text       Loading text.
    void loading(short percentage, QString text = NULL) const;

    /// \brief This signal is emitted to send success message.
    /// \param[in] title Message brief title.
    /// \param[in] text  Message detailed text.
    void success(QString title, QString text) const;

    /// \brief This signal is emitted to send warning message.
    /// \param title Message brief title.
    /// \param text  Message detailed text.
    void warning(QString title, QString text) const;

    /// \brief This signal is emitted to send error message.
    /// \param[in] title Message brief title.
    /// \param[in] text  Message detailed text.
    void error(QString title, QString text) const;

    /// \brief This signal is emitted when APK is successfully unpacked.
    /// \param[in] filename Filename (with full path) of just unpacked APK.
    void unpacked(QString filename) const;

    /// \brief This signal is emitted when APK is successfully packed.
    /// \param[in] filename Filename (with full path) of just packed APK.
    void packed(QString filename) const;

public slots:
    /// \brief Delete temporary files.
    void clearTemp();
};

#endif // PACKER_H
