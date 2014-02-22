/// \file drawarea.h
/// \brief This file contains #Dropbox class declaration.

#ifndef DROPBOX_H
#define DROPBOX_H

#include <QNetworkReply>
#include <QTimer>

/// \brief This class works with Dropbox Core API for uploading APKs.

class Dropbox : public QObject
{
    Q_OBJECT

private:
    QNetworkAccessManager *http;    ///< HTTP manager.
    QTimer *timer;                  ///< Timer to handle connection timeouts.
    QString filename;               ///< Holds filename of a file to upload.
    QString token;                  ///< Holds current user token.
    void auth();                    ///< Opens Dropbox website to sign in and calls #auth_required() signal.
    void parseToken(QString json);  ///< Parse user token from Dropbox JSON reply.
    void doUpload();                ///< Upload file from #filename to Dropbox.

public:
    /// Login using code provided by Dropbox.
    void login(QString code);

    /// Checks if user is logged in, then uploads the file to Dropbox.
    /// \warning This is \b not an actual upload. See #doUpload().
    void upload(QString _filename);

    /// Set Dropbox user token.
    void setToken(QString _token) { token = _token; }

    /// Returns current user token.
    QString getToken() const { return token; }

    explicit Dropbox(QObject *parent = 0);

signals:
    /// Emitted when Dropbox user is not logged in.
    void auth_required();

    /// Emitted when upload progress state is changed.
    void progress(short percentage, QString text);

    /// Emitted on critical error.
    void error(QString title, QString text);

    /// Emitted when upload is finished (successfully or not).
    void uploaded(bool success = true);

    /// Emitted to cancel cancel upload.
    /// \warning For internal purposes. Do not call directly.
    void cancelled();

private slots:
    void catchReply(QNetworkReply *reply);
    void catchError(QNetworkReply::NetworkError code, QString text = NULL);
    void uploadProgress(qint64 sent, qint64 total);
    void timeout();

public slots:
    /// Cancel upload operation.
    void cancel() { timer->stop(); emit cancelled(); }

};

#endif // DROPBOX_H
