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

# If you use a system-font, you have to remove 'rc/font.qrc' from the RESOURCES,
# and edit main.cpp.
# If you use the default-font, you have to install IPA P Gothic font under rc.
# IPA P Gothic is available from http://ipafont.ipa.go.jp/old/ipafont/download.html.
RESOURCES = rc/icon.qrc rc/font.qrc

INCLUDEPATH +=
LIBS += -larchive

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CFLAGS   += 

