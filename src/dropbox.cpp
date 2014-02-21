#include "dropbox.h"
#include "keys.h"
#include <QFile>
#include <QFileInfo>
#include <QDesktopServices>

const QString DROPBOX_AUTH      = "https://www.dropbox.com/1/oauth2/authorize?response_type=code&client_id=855xbc27r2d2y6a";
const QString DROPBOX_TOKEN     = "https://api.dropbox.com/1/oauth2/token";
const QString DROPBOX_ACCOUNT   = "https://api.dropbox.com/1/account/info";
const QString DROPBOX_UPLOAD    = "https://api-content.dropbox.com/1/files_put/sandbox/";
const int TIMEOUT               = 10000;

Dropbox::Dropbox(QObject *parent) : QObject(parent)
{
    http = new QNetworkAccessManager(this);
    timer = new QTimer(this);
    timer->setInterval(TIMEOUT);
    timer->setSingleShot(true);
    connect(http, SIGNAL(finished(QNetworkReply*)), this, SLOT(catchReply(QNetworkReply*)));
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

void Dropbox::catchReply(QNetworkReply *reply)
{
    reply->deleteLater();
    timer->stop();

    if (reply->error() == QNetworkReply::NoError) {

        const int CODE = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (CODE >= 200 && CODE < 300) {
            const QString URL = reply->url().toString();
            if (URL == DROPBOX_TOKEN) {
                QByteArray bytes = reply->readAll();
                parseToken(QString(bytes));
                if (!filename.isEmpty()) {
                    doUpload();
                }
            }
            else if (URL == DROPBOX_ACCOUNT) {
                doUpload();
            }
            else if (URL.contains(DROPBOX_UPLOAD)) {
                emit uploaded();
            }
        }
        else if (CODE >= 300 && CODE < 400) {
            QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            redirect = reply->url().resolved(redirect);
            http->get(QNetworkRequest(QUrl(redirect)));
        }
    }
    else if (reply->error() == QNetworkReply::AuthenticationRequiredError ||
             reply->error() == QNetworkReply::UnknownContentError) {
        auth();
    }
}

void Dropbox::catchError(QNetworkReply::NetworkError reply)
{
    const QString ERR(tr("Error uploading APK to %1.").arg("Dropbox"));
    if (reply != QNetworkReply::OperationCanceledError) {
        emit error(tr("Upload Error"),
                   QString("%1\n%2").arg(ERR, tr("Code: %2")).arg(reply));
        uploaded(false);
    }
}

void Dropbox::uploadProgress(qint64 sent, qint64 total)
{
    if (total != 0) {
        float mbSent = float(sent) / 1024 / 1024;
        float mbTotal = float(total) / 1024 / 1024;
        mbSent = QString::number(mbSent, 'f', 1).toFloat();
        mbTotal = QString::number(mbTotal, 'f', 1).toFloat();
        short percentage = mbSent / mbTotal * 100;
        if (percentage != 100) {
            emit progress(percentage, QString(tr("Uploaded: %1"))
                          .arg("%1 %3 / %2 %3")
                          .arg(mbSent)
                          .arg(mbTotal)
                          .arg(tr("MiB")));
        }
        else {
            emit progress(100, tr("Finishing upload. Please wait..."));
        }
    }
}

void Dropbox::timeout()
{
    const QString ERR(tr("Error uploading APK to %1.").arg("Dropbox"));
    emit error(tr("Upload Error"),
               QString("%1\n%2").arg(ERR, tr("Check your Internet connection.")));
    emit uploaded(false);
    cancel();
}

void Dropbox::auth()
{
    QDesktopServices::openUrl(QUrl(DROPBOX_AUTH));
    emit auth_required();
}

void Dropbox::login(QString code)
{
    QByteArray params;
    params.append(QString("%1=%2&").arg("code", code));
    params.append(QString("%1=%2&").arg("grant_type", "authorization_code"));
    params.append(QString("%1=%2&").arg("client_id", DROPBOX_ID));
    params.append(QString("%1=%2").arg("client_secret", DROPBOX_SECRET));

    QNetworkRequest request;
    request.setUrl(QUrl(DROPBOX_TOKEN));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    http->post(request, params);
}

void Dropbox::parseToken(QString json)
{
    QRegExp rx;
    rx.setMinimal(true);
    rx.setPattern("\\\"access_token\\\": \\\"(.+)\\\"");
    rx.indexIn(json);
    if (rx.capturedTexts().size() > 1) {
        token = rx.capturedTexts().at(1);
    }
}

void Dropbox::upload(QString _filename)
{
    // This is NOT an actual upload!
    // See Dropbox::doUpload()

    emit progress(0, tr("Checking if you are logged in..."));
    filename = _filename;
    timer->start();
    if (!token.isEmpty()) {
        QNetworkRequest request;
        request.setUrl(QUrl(DROPBOX_ACCOUNT));
        request.setRawHeader("Authorization", QString("Bearer " + token).toUtf8());
        http->head(request);
    }
    else {
        auth();
    }
}

void Dropbox::doUpload()
{
    emit progress(0, tr("Uploading to %1...").arg("Dropbox"));

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        error(tr("Upload Error"), tr("Could not open local file."));
        uploaded(false);
        return;
    }
    QByteArray body = file.readAll();
    file.close();
    QFileInfo fi(filename);
    QString title(fi.fileName());

    QNetworkRequest request;
    request.setUrl(QUrl(DROPBOX_UPLOAD + title + "?overwrite=false"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=-----------------------------1a3189a56701e");
    request.setHeader(QNetworkRequest::ContentLengthHeader, body.size());
    request.setRawHeader("Authorization", QString("Bearer " + token).toUtf8());

    QNetworkReply *reply = http->put(request, body);
    connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(uploadProgress(qint64, qint64)));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(catchError(QNetworkReply::NetworkError)));
    connect(this, SIGNAL(cancelled()), reply, SLOT(abort()));
}
