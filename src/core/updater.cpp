#include "updater.h"
#include "globals.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>

Updater::Updater(QObject *parent) : QObject(parent)
{
    http = new QNetworkAccessManager(this);
    connect(http, SIGNAL(finished(QNetworkReply*)), this, SLOT(catchReply(QNetworkReply*)));
}

void Updater::check() const
{
    QNetworkRequest request(Url::VERSION);
    http->get(request);
}

void Updater::download() const
{
    QDesktopServices::openUrl(QUrl(Url::UPDATE));
}

bool Updater::compare(QString v1, QString v2)
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

QString Updater::parse(QString json)
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

void Updater::catchReply(QNetworkReply *reply)
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
}
