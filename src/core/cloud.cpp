#include "cloud.h"
#include "globals.h"
#include <QFileInfo>
#include <QHttpMultiPart>
#include <QDesktopServices>

// Dropbox:
const QString DROPBOX_AUTH      = QString("https://www.dropbox.com/oauth2/authorize?response_type=code&client_id=%1").arg(DROPBOX_ID);
const QString DROPBOX_TOKEN     = "https://api.dropboxapi.com/oauth2/token";
const QString DROPBOX_STATUS    = "https://api.dropboxapi.com/2/users/get_current_account";
const QString DROPBOX_UPLOAD    = "https://content.dropboxapi.com/2/files/upload";

// Google Drive:
const QString GDRIVE_AUTH       = QString("https://accounts.google.com/o/oauth2/auth?response_type=code&scope=https://www.googleapis.com/auth/drive.file&redirect_uri=urn:ietf:wg:oauth:2.0:oob&client_id=%1").arg(GGDRIVE_ID);
const QString GDRIVE_TOKEN      = "https://accounts.google.com/o/oauth2/token";
const QString GDRIVE_REDIRECT   = "urn:ietf:wg:oauth:2.0:oob";
const QString GDRIVE_STATUS     = "https://www.googleapis.com/drive/v2/about";
const QString GDRIVE_FILES      = "https://www.googleapis.com/drive/v2/files";
const QString GDRIVE_UPLOAD     = "https://www.googleapis.com/upload/drive/v2/files";

// OneDrive:
const QString ONEDRIVE_AUTH     = QString("https://login.live.com/oauth20_authorize.srf?client_id=%1&scope=wl.skydrive_update&response_type=code&redirect_uri=http://kefir500.github.io/apk-icon-editor/onedrive").arg(ONEDRIVE_ID);
const QString ONEDRIVE_TOKEN    = "https://login.live.com/oauth20_token.srf";
const QString ONEDRIVE_REDIRECT = Url::WEBSITE + "onedrive";
const QString ONEDRIVE_STATUS   = "https://apis.live.net/v5.0/me";
const QString ONEDRIVE_FILES    = "https://apis.live.net/v5.0/me/skydrive";
const QString ONEDRIVE_UPLOAD   = "https://apis.live.net/v5.0/me/skydrive/files/";

// HTTP:
const int TIMEOUT = 15000;

Cloud::Cloud(QObject *parent) : QObject(parent)
{
    http = new QNetworkAccessManager(this);
    timer = new QTimer(this);
    timer->setInterval(TIMEOUT);
    timer->setSingleShot(true);
    connect(http, SIGNAL(finished(QNetworkReply*)), this, SLOT(catchReply(QNetworkReply*)));
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

void Cloud::cancel()
{
    timer->stop();
    emit finished(false);
    emit cancelled();
}

void Cloud::catchReply(QNetworkReply *reply)
{
    reply->deleteLater();
    timer->stop();

    if (reply->error() == QNetworkReply::NoError) {

        const int CODE = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (CODE >= 200 && CODE < 300) {
            processReply(reply);
        }
        else if (CODE >= 300 && CODE < 400) {
            QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            redirect = reply->url().resolved(redirect);
            http->get(QNetworkRequest(QUrl(redirect)));
        }
    }
    else if (reply->error() == QNetworkReply::AuthenticationRequiredError) {
        if (!reply->url().toString().contains(DROPBOX_UPLOAD)) {
            auth();
        }
        else {
            // Handle situation when the destination Dropbox folder is removed during upload.
            catchError(reply->error(), reply->errorString());
        }
    }
    else if (reply->error() == QNetworkReply::UnknownContentError
             && reply->url().toString().contains(urlToken))
    {
        catchError(reply->error(), tr("Check the code you entered."));
    }
    else {
#ifdef QT_DEBUG
        qDebug() << "------- Reply -------";
        qDebug() << reply->url();
        qDebug() << reply->readAll();
        qDebug() << "----- Reply End -----";
#endif
        catchError(reply->error(), reply->errorString());
    }
}

bool Cloud::processReply(QNetworkReply *reply)
{
    const QString URL = reply->url().toString();

    if (URL == urlToken) {
        const QString REPLY = reply->readAll();
        parseToken(REPLY);
        if (!filename.isEmpty() && !token.isEmpty()) {
            upload(filename);
        }
        return true;
    }
    else if (URL.contains(urlUpload)) {
        emit finished(true);
        return true;
    }
    return false;
}

void Cloud::catchError(QNetworkReply::NetworkError code, QString text)
{
    if (code != QNetworkReply::OperationCanceledError) {
        const QString ERR_UPLOAD(tr("Error uploading APK to %1.").arg(title));
        QString errtext;
        if (!text.isEmpty()) {
            errtext = QString("%1\n%2").arg(ERR_UPLOAD, tr(text.toStdString().c_str()));
        }
        else {
            errtext = QString("%1\n%2").arg(ERR_UPLOAD, tr("Code: %1").arg(code));
        }
        emit error(tr("Upload Error"), errtext);
        emit finished(false);
    }
}

void Cloud::uploadProgress(qint64 sent, qint64 total)
{
    timer->start();
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

void Cloud::timeout()
{
    cancel();
    const QString ERR = tr("Error uploading APK to %1.").arg(title);
    emit error(tr("Upload Error"), QString("%1\n%2").arg(ERR, tr("Timeout expired. Check your Internet connection.")));
}

void Cloud::auth()
{
    QDesktopServices::openUrl(QUrl(urlAuth));
    emit auth_required();
}

void Cloud::login(QString code)
{
    QByteArray params;
    params.append(QString("%1=%2").arg("code", code));
    params.append(QString("&%1=%2").arg("grant_type", "authorization_code"));
    params.append(QString("&%1=%2").arg("client_id", clientID));
    params.append(QString("&%1=%2").arg("client_secret", secret));
    if (!urlRedirect.isEmpty()) {
        params.append(QString("&%1=%2").arg("redirect_uri", urlRedirect));
    }

    QNetworkRequest request;
    request.setUrl(QUrl(urlToken));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    timer->start();
    http->post(request, params);
}

void Cloud::parseToken(QString json)
{
    token.clear();
    QRegExp rx;
    rx.setMinimal(true);
    rx.setPattern("\\\"access_token\\\"\\s*:\\s*\\\"(.+)\\\"");
    rx.indexIn(json);
    if (rx.capturedTexts().size() > 1) {
        token = rx.capturedTexts().at(1);
    }
}

QByteArray Cloud::readFile(QString filename)
{
    emit progress(0, tr("Reading local file..."));

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray body = file.readAll();
        file.close();
        return body;
    }
    else {
        emit error(tr("Upload Error"), tr("Could not open local file."));
        emit finished(false);
        return NULL;
    }
}

void Cloud::upload(QString filename)
{
    // This is NOT an actual upload!
    // See Uploader::startUpload()

    qDebug() << "Uploading to" << title;
    emit progress(0, tr("Checking if you are logged in..."));
    this->filename = filename;
    if (!token.isEmpty()) {
        QNetworkRequest request;
        request.setUrl(QUrl(urlStatus));
        request.setRawHeader("Authorization", QString("Bearer " + token).toUtf8());
        timer->start();
        if (qobject_cast<Dropbox*>(this)) {
            request.setRawHeader("Content-Type", "application/json");
            http->post(request, "null");
        } else {
            http->head(request);
        }
    }
    else {
        auth();
    }
}

// Dropbox

Dropbox::Dropbox(QObject *parent) : Cloud(parent)
{
    setTitle("Dropbox");
    setClientID(DROPBOX_ID);
    setSecret(DROPBOX_SECRET);
    setUrlAuth(DROPBOX_AUTH);
    setUrlToken(DROPBOX_TOKEN);
    setUrlStatus(DROPBOX_STATUS);
    setUrlUpload(DROPBOX_UPLOAD);
}

bool Dropbox::processReply(QNetworkReply *reply)
{
    if (!Cloud::processReply(reply)) {

        // Process Dropbox specific replies.
        const QString URL = reply->url().toString();
        if (URL == urlStatus) {
            startUpload();
            return true;
        }
    }
    return false;
}

void Dropbox::startUpload()
{
    const QByteArray FILE = readFile(filename);
    if (FILE.isEmpty()) return;

    const QString TITLE = QFileInfo(filename).fileName();

    QNetworkRequest request;
    request.setUrl(QUrl(urlUpload));
    request.setRawHeader("Authorization", QString("Bearer " + token).toUtf8());
    request.setRawHeader("Content-Type", "application/octet-stream");
    request.setRawHeader("Dropbox-API-Arg", QString("{\"path\": \"/%1\", \"mode\": \"add\", \"autorename\": true}").arg(TITLE).toUtf8());

    timer->start();
    QNetworkReply *reply = http->post(request, FILE);
    connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(uploadProgress(qint64, qint64)));
    connect(this, SIGNAL(cancelled()), reply, SLOT(abort()));
}

// Google Drive

GoogleDrive::GoogleDrive(QObject *parent) : Cloud(parent)
{
    setTitle("Google Drive");
    setClientID(GGDRIVE_ID);
    setSecret(GGDRIVE_SECRET);
    setUrlAuth(GDRIVE_AUTH);
    setUrlToken(GDRIVE_TOKEN);
    setUrlRedirect(GDRIVE_REDIRECT);
    setUrlStatus(GDRIVE_STATUS);
    setUrlFiles(GDRIVE_FILES);
    setUrlUpload(GDRIVE_UPLOAD);
}

bool GoogleDrive::processReply(QNetworkReply *reply)
{
    if (!Cloud::processReply(reply)) {

        // Process Google Drive specific replies.

        const QString URL = reply->url().toString();
        if (URL.contains(urlFiles)) {
            QString r = reply->readAll();
            QRegExp rx;
            rx.setMinimal(true);
            rx.setPattern("\"id\": \"(.+)\"");
            rx.indexIn(r);
            if (rx.capturedTexts().size() > 1) {
                folder = rx.capturedTexts().at(1);
            }
            if (!folder.isEmpty()) {
                // Folder found!
                startUpload();
                return true;
            }
            else {
                // Creating new folder...
                createFolder();
                return false;
            }
        }
        else if (URL == urlStatus) {
            getFolderID();
            return true;
        }
    }
    return false;
}

void GoogleDrive::getFolderID()
{
    const QString QUERY = QString("title = '%1' and mimeType = 'application/vnd.google-apps.folder' and trashed = false").arg(APP);
    QNetworkRequest request;
    request.setUrl(QUrl(urlFiles + QString("?q=%1").arg(QUERY)));
    request.setRawHeader("Authorization", QString("Bearer " + token).toUtf8());
    timer->start();
    http->get(request);
}

void GoogleDrive::createFolder()
{
    const QByteArray JSON = QString("{\"title\":\"%1\",\"mimeType\":\"application/vnd.google-apps.folder\"}").arg(APP).toUtf8();
    QNetworkRequest request;
    request.setUrl(QUrl(urlFiles));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, JSON.size());
    request.setRawHeader("Authorization", QString("Bearer " + token).toUtf8());
    timer->start();
    http->post(request, JSON);
}

void GoogleDrive::startUpload()
{
    const QByteArray BYTES = readFile(filename);
    if (BYTES.isEmpty()) {
        return;
    }
    QFileInfo fi(filename);
    const QString TITLE = fi.fileName();
    const QString JSON = QString("{\"title\":\"%1\",\"parents\":[{\"id\":\"%2\"}]}").arg(TITLE, folder);

    QHttpPart part1;
    part1.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");
    part1.setBody(JSON.toStdString().c_str());

    QHttpPart part2;
    part2.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    part2.setBody(BYTES);

    QHttpMultiPart *multi = new QHttpMultiPart(QHttpMultiPart::RelatedType);
    multi->append(part1);
    multi->append(part2);

    QNetworkRequest request;
    request.setUrl(QUrl(urlUpload + "?uploadType=multipart"));
    request.setRawHeader("Authorization", QString("Bearer " + token).toUtf8());

    timer->start();
    QNetworkReply *reply = http->post(request, multi);
    multi->setParent(reply);
    connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(uploadProgress(qint64, qint64)));
    connect(this, SIGNAL(cancelled()), reply, SLOT(abort()));
}

// OneDrive

OneDrive::OneDrive(QObject *parent) : Cloud(parent)
{
    setTitle("Microsoft OneDrive");
    setClientID(ONEDRIVE_ID);
    setSecret(ONEDRIVE_SECRET);
    setUrlAuth(ONEDRIVE_AUTH);
    setUrlToken(ONEDRIVE_TOKEN);
    setUrlRedirect(ONEDRIVE_REDIRECT);
    setUrlStatus(ONEDRIVE_STATUS);
    setUrlFiles(ONEDRIVE_FILES);
    setUrlUpload(ONEDRIVE_UPLOAD);
}

bool OneDrive::processReply(QNetworkReply *reply)
{
    if (!Cloud::processReply(reply)) {

        // Process OneDrive specific replies.
        const QString URL = reply->url().toString();
        if (URL == urlStatus) {
            startUpload();
            return true;
        }
    }
    return false;
}

void OneDrive::getFolderID()
{
    /*QNetworkRequest request;
    request.setUrl(QUrl(urlFiles));
    request.setRawHeader("Authorization", QString("Bearer " + token).toUtf8());
    http->get(request);*/
}

void OneDrive::createFolder()
{
    /*QByteArray json = QString("{\"name\":\"%1\"}").arg(APP).toUtf8();
    QNetworkRequest request;
    request.setUrl(QUrl(urlFiles));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, json.size());
    request.setRawHeader("Authorization", QString("Bearer " + token).toUtf8());
    http->post(request, json);*/
}

void OneDrive::startUpload()
{
    const QByteArray BYTES = readFile(filename);
    if (BYTES.isEmpty()) {
        return;
    }
    QFileInfo fi(filename);
    const QString FILENAME = fi.fileName();

    QNetworkRequest request;
    request.setUrl(QUrl(urlUpload + FILENAME + "?overwrite=ChooseNewName"));
    request.setRawHeader("Authorization", QString("Bearer " + token).toUtf8());

    timer->start();
    QNetworkReply *reply = http->put(request, BYTES);
    connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(uploadProgress(qint64, qint64)));
    connect(this, SIGNAL(cancelled()), reply, SLOT(abort()));
}
