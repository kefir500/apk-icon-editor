#include "updater.h"
#include "globals.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>
#include <QThread>

// Updater

void Updater::check() const
{
    QThread *thread = new QThread();
    UpdateWorker *worker = new UpdateWorker();
    worker->moveToThread(thread);

    connect(thread, SIGNAL(started()), worker, SLOT(check()));
    connect(worker, SIGNAL(version(QString)), this, SIGNAL(version(QString)));
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void Updater::download() const
{
    QDesktopServices::openUrl(QUrl(Url::UPDATE));
}

// UpdateWorker

UpdateWorker::UpdateWorker(QObject *parent) : QObject(parent)
{
    http = new QNetworkAccessManager(this);
    connect(http, SIGNAL(finished(QNetworkReply*)), this, SLOT(catchReply(QNetworkReply*)));
}

void UpdateWorker::check() const
{
    QNetworkRequest request(Url::VERSION);
    http->get(request);
}

void UpdateWorker::catchReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {

        const int CODE = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (CODE >= 200 && CODE < 300) {
            const QString URL = reply->url().toString();
            if (URL.indexOf(Url::VERSION) != -1) {

                const QString JSON = reply->readAll().trimmed();
                const QString VERSION = parse(JSON);
                if (compare(VERSION, VER)) {
                    emit version(VERSION);
                }
            }
        }
    }

    reply->deleteLater();
    emit finished();
}

bool UpdateWorker::compare(QString v1, QString v2) const
{
    // Unit test is available for this function.

    QStringList segments1 = v1.split('.');
    QStringList segments2 = v2.split('.');

    for (short i = 0; i < segments1.size(); ++i) {

        if (segments2.size() <= i) {
            segments2.push_back("0");
        }

        const int SEGMENT1 = segments1.at(i).toInt();
        const int SEGMENT2 = segments2.at(i).toInt();

        if (SEGMENT1 == SEGMENT2) {
            continue;
        }
        else {
            return (SEGMENT1 > SEGMENT2);
        }
    }
    return false;
}

QString UpdateWorker::parse(QString json) const
{
#if defined(Q_OS_WIN)
    const QString OS = "windows";
#elif defined(Q_OS_OSX)
    const QString OS = "osx";
#else
    const QString OS = "windows";
#endif
    const QString LATEST = QJsonDocument::fromJson(json.toUtf8())
                           .object()["1.3.0"]
                           .toObject()[OS]
                           .toObject()["version"].toString();

    qDebug() << qPrintable(QString("Updater: v%1\n").arg(LATEST));
    return LATEST;
}
