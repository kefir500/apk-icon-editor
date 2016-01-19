///
/// \file
/// This file contains classes used for update checks.
///

#ifndef UPDATER_H
#define UPDATER_H

#include <QNetworkAccessManager>

///
/// Update check.
/// This class performs an online check for application updates.
/// \warning Some firewalls cause a slight delay at the time of the first check,
///          that is why it is recommended to use the wrapper instead (#Updater class).
///

class UpdateWorker : public QObject {
    Q_OBJECT

public:
    explicit UpdateWorker(QObject *parent = 0);

public slots:

    /// Checks for application updates.
    void check() const;

    /// Compares the application version numbers.
    /// \return Returns \c true if version \c v1 is newer than version \c v2.
    /// \retval true if version \c v1 is newer than version \c v2.
    /// \retval false if version \c v1 is the same or older than version \c v2.
    /// \note Unit test is available for this function.
    bool compare(QString v1, QString v2);

signals:
    /// This signal is emitted when the application update is available.
    /// \param version Version number of the new release.
    void version(QString version);

    /// This signal is emitted after the update check is done.
    void finished();

private:
    QString parse(QString json); ///< Returns the version number parsed from the incoming \c json.
    QNetworkAccessManager *http; ///< HTTP request manager.

private slots:
    void catchReply(QNetworkReply *reply); ///< Handles the HTTP \c reply.
};

///
/// Asynchronous update check.
/// This class is a wrapper for the #UpdateWorker class.
/// It performs an online check for application updates in a separate thread.
///

class Updater : public QObject {
    Q_OBJECT

public:
    explicit Updater(QObject *parent = 0) : QObject(parent) {}

public slots:
    void check() const;    ///< Checks for application updates.
    void download() const; ///< Opens download URL in the default browser.

signals:
    /// This signal is emitted when the application update is available.
    /// \param version Version number of the new release.
    void version(QString version);
};

#endif // UPDATER_H
