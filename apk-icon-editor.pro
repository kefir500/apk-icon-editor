TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = simplecrypt apk-icon-editor

apk-icon-editor.file = $$PWD/src/apk-icon-editor.pro
simplecrypt.file     = $$PWD/lib/src/simplecrypt-3/SimpleCrypt.pro

OTHER_FILES += .gitignore .travis.yml appveyor.yml
