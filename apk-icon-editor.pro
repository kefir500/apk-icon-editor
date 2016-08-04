TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = quazip simplecrypt apk-icon-editor

apk-icon-editor.file = $$PWD/src/apk-icon-editor.pro
quazip.file          = $$PWD/lib/src/quazip-0.7/quazip/quazip.pro
simplecrypt.file     = $$PWD/lib/src/simplecrypt-3/SimpleCrypt.pro

unix:!macx {
    share.files   = $$PWD/bin/linux/*
    share.path    = /usr/share/apk-icon-editor
    icons.files   = $$PWD/res/icons/linux/apk-icon-editor/*
    icons.path    = /usr/share/icons/hicolor
    desktop.files = $$PWD/res/icons/linux/apk-icon-editor.desktop
    desktop.path  = /usr/share/applications
    INSTALLS      += share icons desktop
}

OTHER_FILES += .gitignore .travis.yml appveyor.yml
