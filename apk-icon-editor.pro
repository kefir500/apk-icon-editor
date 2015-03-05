TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = quazip simplecrypt apk-icon-editor

apk-icon-editor.file = src/apk-icon-editor.pro
quazip.file          = lib/src/quazip-0.7/quazip/quazip.pro
simplecrypt.file     = lib/src/simplecrypt-3/SimpleCrypt.pro
