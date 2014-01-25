#include "updater.h"
#include "main.h"
#include <QApplication>
#include <QDesktopServices>
#include <QProcess>

#ifdef Q_OS_WIN32
    #include <windows.h>
#endif

Updater::Updater(QObject *parent) : QObject(parent)
{
    http = new QNetworkAccessManager(this);
    connect(http, SIGNAL(finished(QNetworkReply*)), this, SLOT(catchReply(QNetworkReply*)));
}

void Updater::check()
{
    QNetworkRequest request(URL_VERSION);
    http->get(request);
}

void Updater::download()
{
    QDesktopServices::openUrl(QUrl(URL_UPDATE));
}

bool Updater::compare(QString v1, QString v2)
{
    // Unit test is available fot this function.

    QStringList list1 = v1.split('.');
    QStringList list2 = v2.split('.');
    for (short i = 0; i < list1.size(); ++i) {

        if (list2.size() <= i) {
            list2.push_back("0");
        }

        int c1 = list1.at(i).toInt();
        int c2 = list2.at(i).toInt();

        if (c1 == c2) {
            continue;
        } else {
            return (c1 > c2);
        }
    }
    return false;
}

void Updater::catchReply(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() == QNetworkReply::NoError) {

        int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (code >= 200 && code < 300) {

            QString url = reply->url().toString();
            if (url.indexOf(URL_VERSION) != -1) {

                QString v(reply->readAll().trimmed());
                if (compare(v, VER)) {
                    emit version(v);
                }
            }
        }
    }
}
