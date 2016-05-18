QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BookReader
TEMPLATE = app

SOURCES += main.cpp\
mainwindow.cpp \
settingscaledialog.cpp \
imageviewer.cpp \
settingsdialog.cpp \
histgramdialog.cpp \
appsettings.cpp \
image.cpp

HEADERS += \
nullptr.hpp \
appinfo.hpp \
histgramdialog.hpp \
imageviewer.hpp \
mainwindow.hpp \
settingscaledialog.hpp \
settingsdialog.hpp \
appsettings.hpp \
image.hpp

FORMS +=

RESOURCES = rc/icon.qrc

QMAKE_CXXFLAGS += -mfma -mavx2 #-g -O0 -fno-inline
QMAKE_CFLAGS   += -mfma -mavx2 #-g -O0 -fno-inline

