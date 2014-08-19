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
    playlistdock.cpp \
    histgramdialog.cpp \
    benchmarkdialog.cpp \
    appsettings.cpp

HEADERS  += \
    nullptr.hpp \
    applicationinfo.hpp \
    histgramdialog.hpp \
    imageviewer.hpp \
    mainwindow.hpp \
    playlistdock.hpp \
    settingscaledialog.hpp \
    settingsdialog.hpp \
    versiondialog.hpp \
    benchmarkdialog.hpp \
    appsettings.hpp

FORMS    +=
