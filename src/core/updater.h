///
/// \file
/// This file contains the class to check for updates.
///

#ifndef UPDATER_H
#define UPDATER_H

#include <QNetworkReply>

///
/// Update check.
/// This class performs an online check for application updates.
///

class Updater : public QObject {
    Q_OBJECT

public:
    explicit Updater(QObject *parent = 0);

public slots:
    void check() const;    ///< Checks for application updates.
    void download() const; ///< Opens download URL in the default browser.

    /// Compares the application version numbers.
    /// \return Returns \c true if version \c v1 is newer than version \c v2.
    /// \retval true if version \c v1 is newer than version \c v2.
    /// \retval false if version \c v1 is the same or older than version \c v2.
    /// \note Unit test is available for this function.
    bool compare(QString v1, QString v2);

private:
    QNetworkAccessManager *http;           ///< HTTP request manager.
    QString parse(QString json);           ///< Returns the version number parsed from the incoming \c json.

private slots:
    void catchReply(QNetworkReply *reply); ///< Handles HTTP \c reply.

signals:
    /// This signal is emitted when the application update is available.
    /// \param version Version number of the new release.
    void version(QString version);
};

#endif // UPDATER_H
