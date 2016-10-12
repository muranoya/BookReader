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
TextEncDialog.cpp

HEADERS += \
MainWindow.hpp \
AppSettings.hpp \
ImageViewer.hpp \
image.hpp \
ScaleDialog.hpp \
SettingDialog.hpp \
TextEncDialog.hpp

FORMS +=

RESOURCES = rc/icon.qrc

# If you use a custom-font, you have to install a font under rc and
# edit the rc/font.qrc and the main.cpp.
USE_CUSTOM_FONT=1

USE_CUSTOM_FONT {
DEFINES += USE_CUSTOM_FONT
RESOURCES += rc/font.qrc
}

INCLUDEPATH +=
LIBS += -larchive

win32 {
QMAKE_CXXFLAGS += -std:c++latest
QMAKE_CFLAGS   += 
}
else:unix {
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CFLAGS   += 
}

