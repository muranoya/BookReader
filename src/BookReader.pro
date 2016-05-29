QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BookReader
TEMPLATE = app

SOURCES += \
main.cpp\
mainwindow.cpp \
appsettings.cpp \
imageviewer.cpp \
image.cpp \
settingscaledialog.cpp \
settingsdialog.cpp \
histgramdialog.cpp

HEADERS += \
nullptr.hpp \
appinfo.hpp \
mainwindow.hpp \
appsettings.hpp \
imageviewer.hpp \
image.hpp \
settingscaledialog.hpp \
settingsdialog.hpp \
histgramdialog.hpp

FORMS +=

RESOURCES = rc/icon.qrc

LIBS += -larchive

QMAKE_CXXFLAGS += -mfma -mavx2
QMAKE_CFLAGS   += -mfma -mavx2

