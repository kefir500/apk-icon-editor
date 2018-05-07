#ifndef APKFILE_H
#define APKFILE_H

#include "apk.h"
#include "iconsmodel.h"
#include "manifestmodel.h"
#include "titlesmodel.h"
#include "device.h"
#include <QDomDocument>

namespace Apk {

    class File {

    public:
        explicit File(const QString &contentsPath);
        ~File();

        void saveIcons();
        void saveTitles();
        void removeFiles();

        bool addIcon(Icon::Type type);
        void removeIcon(Icon *icon);

        QString getFilePath() const;
        QString getContentsPath() const;
        QIcon getThumbnail() const;

        bool getApksigner() const;
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

        void setFilePath(QString filePath);
        void setApksigner(bool value);
        void setSmali(bool value);
        void setSign(bool value);
        void setZipalign(bool value);        
        void setKeystore(bool value);

        void setFilePemPk8(QString pem, QString pk8);
        void setFileKeystore(QString filePath, QString alias, QString passKeystore, QString passAlias);

        IconsModel iconsModel;
        ManifestModel manifestModel;
        TitlesModel titlesModel;

    private:
        QString getIconPath(Icon::Type type);

        QString filePath;     ///< APK filename.
        QString contentsPath; ///< Path to APK contents directory.

        QIcon thumbnail;
        Manifest *manifest;

        bool isApksigner;     ///< If \c true, apksigner is used instead of signapk / jarsigner.
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
    };
}

#endif // APKFILE_H
