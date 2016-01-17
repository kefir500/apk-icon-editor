#ifndef APKFILE_H
#define APKFILE_H

#include "apk.h"
#include "icon.h"
#include "device.h"

namespace Apk {

    class File {

    public:
        ~File();

        void clear();

        QString getFilePath() const;
        QString getDirectory() const;
        QString getAppTitle() const;
        QString getVarAppTitle() const;
        QString getVersionName() const;
        QString getVersionCode() const;

        Icon* getIcon(Dpi::Type dpi) const;
        QList<QSharedPointer<Icon> > getIcons() const;
        QList<Apk::String> getStrings() const;

        short getRatio() const;
        bool getApktool() const;
        bool getSmali() const;
        bool getSign() const;
        bool getZipalign() const;
        bool getKeystore() const;

        QString getFilePem() const;
        QString getFilePk8() const;
        QString getFileKeystore() const;
        QString getPassKeystore() const;
        QString getAlias() const;
        QString getPassAlias() const;

        void setFilePath(QString filepath);
        void setDirectory(QString path); // Warning: unsafe method
        void setAppTitle(QString title);
        void setVarAppTitle(QString variable);
        void setVersionName(QString name);
        void setVersionCode(QString code);

        void setIcons(QList<QSharedPointer<Icon> > icons);
        void setStrings(QList<Apk::String> strings);

        void setApktool(bool value);
        void setRatio(short ratio);
        void setSmali(bool value);
        void setSign(bool value);
        void setZipalign(bool value);        
        void setKeystore(bool value);

        void setFilePemPk8(QString pem, QString pk8);
        void setFileKeystore(QString filepath, QString alias, QString passKeystore, QString passAlias);

    private:
        QString filepath;     ///< APK filename.
        QString contents;     ///< Path to APK contents directory.
        QString appTitle;     ///< Application title.
        QString varAppTitle;  ///< Name of the application title variable.
        QString versionName;  ///< Application version name.
        QString versionCode;  ///< Application version code.

        short ratio;          ///< ZIP compression ratio [0-9] (only in ZIP mode).
        bool isApktool;       ///< If \c true, Apktool is used instead of ZIP.
        bool isSmali;         ///< If \c true, decompile "classes.dex" (only in Apktool mode).
        bool isSign;          ///< If \c true, sign APK.
        bool isZipalign;      ///< If \c true, zipalign APK.
        bool isKeystore;      ///< If \c true, use KeyStore singning method instead of PEM/PK8.

        QString filePem;      ///< Path to PEM file.
        QString filePk8;      ///< Path to PK8 file.
        QString fileKeystore; ///< Path to KeyStore file.
        QString passKeystore; ///< KeyStore password.
        QString alias;        ///< KeyStore alias.
        QString passAlias;    ///< KeyStore alias password.

        QList<QSharedPointer<Icon> > icons; ///< List of APK icons.
        QList<Apk::String> strings;         ///< List of APK strings.
    };
}

#endif // APKFILE_H
