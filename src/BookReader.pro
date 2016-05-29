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
HistgramDialog.cpp

HEADERS += \
nullptr.hpp \
appinfo.hpp \
MainWindow.hpp \
AppSettings.hpp \
ImageViewer.hpp \
image.hpp \
ScaleDialog.hpp \
SettingDialog.hpp \
HistgramDialog.hpp

FORMS +=

RESOURCES = rc/icon.qrc

LIBS += -larchive

QMAKE_CXXFLAGS += -mfma -mavx2
QMAKE_CFLAGS   += -mfma -mavx2

