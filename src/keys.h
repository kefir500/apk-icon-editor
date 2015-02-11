/// \file keys.h
/// \brief This file contains private keys.
///
/// This file contains private data used for password encryption and OAuth authentication.
/// The provided variables are DUMMIES.

#ifndef KEYS_H
#define KEYS_H

#include <QString>

#error "[keys.h] The following variables are dummies. Set your own values and/or remove this line."

const quint64 SIMPLECRYPT_KEY   = Q_UINT64_C(0x0000000000000000);

const QString DROPBOX_ID        = "0000000000000000";
const QString DROPBOX_SECRET    = "0000000000000000";

const QString GGDRIVE_ID        = "0000000000000000";
const QString GGDRIVE_SECRET    = "0000000000000000";

const QString ONEDRIVE_ID       = "0000000000000000";
const QString ONEDRIVE_SECRET   = "0000000000000000";

#endif // KEYS_H
