QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SpRead
TEMPLATE = app

SOURCES += \
main.cpp\
MainWindow.cpp \
App.cpp \
Viewer.cpp \
ImageFile.cpp \
PlaylistModel.cpp \
ImageViewer.cpp \
image.cpp \
ScaleDialog.cpp \
SettingDialog.cpp \
Prefetcher.cpp

HEADERS += \
for_windows_env.hpp \
MainWindow.hpp \
App.hpp \
Viewer.hpp \
ImageFile.hpp \
PlaylistModel.hpp \
ImageViewer.hpp \
image.hpp \
ScaleDialog.hpp \
SettingDialog.hpp \
Prefetcher.hpp

FORMS +=

# If you use a custom-font, you have to install a font under rc and
# edit the rc/font.qrc and the main.cpp.
USE_CUSTOM_FONT = 1

equals(USE_CUSTOM_FONT,1) {
DEFINES = USE_CUSTOM_FONT
RESOURCES += rc/font.qrc
}

INCLUDEPATH +=
LIBS += -larchive

win32-msvc* {
QMAKE_CXXFLAGS += -std:c++11
QMAKE_CFLAGS   += 
}
else:win32 {
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CFLAGS   += 
}
else:unix {
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CFLAGS   += 
}

