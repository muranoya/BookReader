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
    appsettings.cpp \
    image.cpp

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
    appsettings.hpp \
    image.hpp

FORMS    +=

QMAKE_CXXFLAGS += -mfma -mavx2
QMAKE_CFLAGS   += -mfma -mavx2
