///
/// \file
/// This file contains classes for uploading files to cloud storage services.
///

#ifndef CLOUD_H
#define CLOUD_H

#include "keys.h"
#include <QNetworkReply>
#include <QTimer>
#include <QIcon>

///
/// Cloud storage uploader.
/// Base class for uploading APK files to cloud storage services.
///

class Cloud : public QObject
{
    Q_OBJECT

protected:
    QString title;                  ///< Cloud storage title.
    QPixmap icon;                   ///< Cloud storage logo.

    QNetworkAccessManager *http;    ///< HTTP manager.
    QTimer *timer;                  ///< Timer to handle connection timeouts.
    QString filename;               ///< Holds filename of a file to upload.
    QString token;                  ///< Holds current user token.

    QString urlAuth;                ///< URL for authentication.
    QString urlStatus;              ///< URL for checking login status.
    QString urlRedirect;            ///< URL for redirection.
    QString urlFiles;               ///< URL for for accessing files and folders.
    QString urlUpload;              ///< URL for for uploading.
    QString urlToken;               ///< URL for retrieving token.
    QString clientID;               ///< Application OAuth 2.0 client ID.
    QString secret;                 ///< Application OAuth 2.0 client secret.

    /// Process the \c reply.
    /// \retval \c TRUE on success.
    virtual bool processReply(QNetworkReply *reply);

    void auth();                            ///< Opens cloud storage website to sign in and calls #auth_required() signal.
    void parseToken(QString json);          ///< Parse user token from cloud storage JSON reply.
    QByteArray readFile(QString filename);  ///< Read bytes from file.
    virtual void startUpload() = 0;         ///< Upload file from #filename to cloud storage. \warning Abstract method.

public:
    /// Log in using the \c code provided by the cloud storage.
    void login(QString code);

    /// Checks if user is logged in, then uploads the file to the cloud storage.
    /// \warning This is \b not an actual upload. See #startUpload().
    void upload(QString filename);

    void setClientID(QString id)     { clientID = id; }      ///< Set application OAuth 2.0 client ID.
    void setSecret(QString _secret)  { secret = _secret; }   ///< Set application OAuth 2.0 client secret.
    void setUrlAuth(QString url)     { urlAuth = url; }      ///< Set URL for authentication.
    void setUrlStatus(QString url)   { urlStatus = url; }    ///< Set URL for checking login status.
    void setUrlRedirect(QString url) { urlRedirect = url; }  ///< Set URL for redirection.
    void setUrlFiles(QString url)    { urlFiles = url; }     ///< Set URL for accessing files and folders.
    void setUrlUpload(QString url)   { urlUpload = url; }    ///< Set URL for uploading.
    void setUrlToken(QString url)    { urlToken = url; }     ///< Set URL for retrieving token.
    void setToken(QString _token)    { token = _token; }     ///< Set current user token value.
    QString getToken() const         { return token; }       ///< Returns current user token value.

    void setTitle(QString _title)    { title = _title; }     ///< Set cloud storage title.
    void setIcon(QPixmap _icon)      { icon = _icon; }       ///< Set cloud storage logo.
    QString getTitle() const         { return title; }       ///< Returns cloud storage title.
    QPixmap getIcon() const          { return icon; }        ///< Returns cloud storage logo.

    explicit Cloud(QObject *parent = 0);

signals:
    /// Emitted when cloud storage user is not logged in.
    void auth_required();

    /// Emitted when upload progress state is changed.
    void progress(short percentage, QString text);

    /// Emitted on critical error.
    void error(QString title, QString text);

    /// Emitted when upload is finished (successfully or not).
    void finished(bool success);

    /// Emitted to cancel upload.
    /// \warning For internal purposes. Do not call directly.
    void cancelled();

public slots:
    /// Cancel upload operation.
    void cancel();

private slots:
    void catchReply(QNetworkReply *reply);
    void catchError(QNetworkReply::NetworkError code, QString text = QString());
    void uploadProgress(qint64 sent, qint64 total);
    void timeout();
};

///
/// Dropbox uploader.
/// Class for uploading files to Dropbox cloud storage service.
///

class Dropbox : public Cloud
{
    Q_OBJECT

public:
    explicit Dropbox(QObject *parent = 0);

private:
    virtual bool processReply(QNetworkReply *reply);
    virtual void startUpload();
};

///
/// Google Drive uploader.
/// Class for uploading files to Google Drive cloud storage service.
///

class GoogleDrive : public Cloud
{
    Q_OBJECT

public:
    explicit GoogleDrive(QObject *parent = 0);

private:
    virtual bool processReply(QNetworkReply *reply);
    virtual void startUpload();
    void getFolderID();         ///< Retrieve "APK Icon Editor" folder ID at Google Drive.
    void createFolder();        ///< Create "APK Icon Editor" folder at Google Drive.
    QString folder;             ///< "APK Icon Editor" folder ID at Google Drive.
};

///
/// OneDrive uploader.
/// Class for uploading files to Microsoft OneDrive cloud storage service.
///

class OneDrive : public Cloud
{
    Q_OBJECT

public:
    explicit OneDrive(QObject *parent = 0);

private:
    virtual bool processReply(QNetworkReply *reply);
    virtual void startUpload();
    void getFolderID();         ///< \warning Dummy method. Do not use.
    void createFolder();        ///< \warning Dummy method. Do not use.
    QString folder;             ///< "APK Icon Editor" folder ID at OneDrive.
};

#endif // UPLOADER_H
