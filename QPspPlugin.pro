#-------------------------------------------------
#
# Project created by QtCreator 2013-06-05T22:27:39
#
#-------------------------------------------------

QT       += core gui

CONFIG += plugin

CONFIG(debug, debug|release) {
    TARGET = qpspd
}
CONFIG(release, debug|release) {
    TARGET = qpsp
}

TEMPLATE = lib

DESTDIR = $$[QT_INSTALL_PLUGINS]/imageformats

SOURCES += qpspplugin.cpp \
    qpsphandler.cpp

HEADERS += qpspplugin.h \
    qpsphandler.h
OTHER_FILES += QPspPlugin.json \
    LICENSE.LGPL \
    README.md

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
