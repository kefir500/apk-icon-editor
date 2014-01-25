/// \file apk.h
/// \brief This file contains #Apk class declaration.

#ifndef PACKER_H
#define PACKER_H

#include "icon.h"
#include "profile.h" // for Dpi enum
#include <QThread>

/// \brief APK manager class.
///
/// Class for performing operations on APK files: #pack, #unpack, #sign and #optimize.<br>
/// Internal processes run in the separate #thread.
///
/// \image html apk.png

class Apk : public QObject {
    Q_OBJECT

private:
    QString filename;                   ///< Current APK filename.
    QString manifest;                   ///< Stores AndroidManifest.xml.
    QList<QSharedPointer<Icon>> icons;  ///< Stores [shared pointers to] loaded APK icons.
    QThread *thread;                    ///< Separate thread to #pack/#unpack.

    QString strError;                   ///< Holds static error text ("%APP% error").
    QString strErrorStart;              ///< Holds static error text ("Error starting %APP%").

    /// \brief This function has to be called on fatal error. Kills active thread and emits #error signal.
    /// \param[in] title Error message brief title.
    /// \param[in] text  Error message detailed text.
    /// \return This function always returns FALSE.
    bool die(QString title, QString text) const;

    /// \brief Checks if 7za worked successfully by its exit code.
    /// \param[in] code 7za exit code.
    /// \retval TRUE if 7za succesfully zipped/unzipped APK (#code equals 0).
    /// \retval FALSE if 7za did not finish successfully (#code is non-zero).
    bool getZipSuccess(int code) const;

    // Unpacking APK:
    bool readManifest();    ///< Read AndroidManifest.xml and save it to #manifest variable.
    bool unzip() const;     ///< Unpack APK to temporary directory using "7za" tool.
    void loadIcons();       ///< Load APK icons.

    // Packing APK:
    bool saveIcons() const;             ///< Save icons from #icons list to PNG images.
    bool zip() const;                   ///< Pack APK using "7za" tool.
    bool checkJavaInstalled() const;    ///< Check if Java Runtime Environment is installed.
    bool sign() const;                  ///< Sign APK using "signapk" tool.
    bool optimize() const;              ///< Optimize APK (optional) using "zipalign" tool.

public:
    /// \brief Replace one of APK icons stored at #icons list.
    /// \param[in] id   DPI of icon to replace.
    /// \param[in] icon Icon to replace with.
    void setIcon(Dpi id, Icon *icon);

    /// \brief Get APK icon from #icons list.
    /// \param[in] id DPI of the required icon.
    /// \return Requested #icon.
    Icon *getIcon(Dpi id) const;

    /// \brief Update QString translations.
    void retranslate();

    Apk();

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

private slots:
    /// \brief Start immediate APK unpacking.
    /// \return TRUE on success.
    /// \warning Function should return FALSE only if #die method was called before.
    /// \todo Probably such behavior (see \b warning) needs some refactoring.
    bool doUnpack();

    /// \brief Start immediate APK packing.
    /// \return TRUE on success.
    /// \warning Function should return FALSE only if #die method was called before.
    /// \todo Probably such behavior (see \b warning) needs some refactoring.
    bool doPack();

public slots:
    /// \brief Unpack APK by calling private #doUnpack method in a separate #thread.
    /// \param[in] _filename Name of APK file to unpack.
    void unpack(QString _filename);

    /// \brief Pack APK by calling private #doPack method in a separate #thread.
    /// \param[in] _filename Name of APK file to pack.
    void pack(QString _filename);

    /// \brief Delete temporary files.
    void clearTemp();
};

#endif // PACKER_H
