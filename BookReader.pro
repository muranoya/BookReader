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
    settings.cpp \
    versiondialog.cpp \
    settingrotatedialog.cpp \
    imageviewer.cpp

HEADERS  += mainwindow.h \
    nullptr.h \
    settingscaledialog.h \
    settings.h \
    versiondialog.h \
    settingrotatedialog.h \
    imageviewer.h \
    applicationinfo.h

FORMS    += mainwindow.ui \
    settingscaledialog.ui \
    versiondialog.ui \
    settingrotatedialog.ui
