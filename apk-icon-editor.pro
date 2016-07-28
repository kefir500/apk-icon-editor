TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = quazip simplecrypt apk-icon-editor

apk-icon-editor.file = $$PWD/src/apk-icon-editor.pro
quazip.file          = $$PWD/lib/src/quazip-0.7/quazip/quazip.pro
simplecrypt.file     = $$PWD/lib/src/simplecrypt-3/SimpleCrypt.pro

OTHER_FILES += .gitignore .travis.yml appveyor.yml
