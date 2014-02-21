/// \file updater.h
/// \brief This file contains #Updater class declaration.

#ifndef UPDATER_H
#define UPDATER_H

#include <QNetworkReply>
#include <QUrl>

/// \brief Application update checker.
///
/// Class to check for application updates online.

class Updater : public QObject {
    Q_OBJECT

private:
    QNetworkAccessManager *http;    ///< Used for HTTP GET requests.

public:
    explicit Updater(QObject *parent = 0);

private slots:
    void catchReply(QNetworkReply *reply);  ///< Catch HTTP reply.

public slots:
    void check();       ///< Check for updates.
    void download();    ///< Download the latest version.

    /// \brief Compare which version number is greater.
    ///
    /// This function checks if \c v1 is greater then \c v2.
    /// Note that <b>%1.%2.%3.%N</b> scheme is used to represent application version.
    /// Only numbers and dots are acceptable.
    /// \retval TRUE if \c v1 > \c v2.
    /// \retval FALSE if \c v1 <= \c v2.
    /// \note Unit test is available for this function.
    bool compare(QString v1, QString v2);

signals:
    /// \brief This signal is emitted after calling #check if new version is available.
    /// \param version String representing version number.
    void version(QString version);

};

#endif // UPDATER_H
