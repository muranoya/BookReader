#-------------------------------------------------
#
# Project created by QtCreator 2014-01-30T02:43:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BookReader
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    settingscaledialog.cpp \
    versiondialog.cpp \
    imageviewer.cpp \
    settingsdialog.cpp \
    playlistdock.cpp

HEADERS  += mainwindow.h \
    nullptr.h \
    settingscaledialog.h \
    settings.h \
    versiondialog.h \
    imageviewer.h \
    applicationinfo.h \
    settingsdialog.h \
    playlistdock.h

FORMS    += mainwindow.ui \
    settingscaledialog.ui \
    versiondialog.ui \
    settingsdialog.ui
