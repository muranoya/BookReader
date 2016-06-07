QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BookReader
TEMPLATE = app

SOURCES += \
main.cpp\
MainWindow.cpp \
AppSettings.cpp \
ImageViewer.cpp \
image.cpp \
ScaleDialog.cpp \
SettingDialog.cpp \
HistgramDialog.cpp \
TEncodingDialog.cpp

HEADERS += \
appinfo.hpp \
MainWindow.hpp \
AppSettings.hpp \
ImageViewer.hpp \
image.hpp \
ScaleDialog.hpp \
SettingDialog.hpp \
HistgramDialog.hpp \
TEncodingDialog.hpp

FORMS +=

RESOURCES = rc/icon.qrc

INCLUDEPATH +=
LIBS += -larchive

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CFLAGS   += 

