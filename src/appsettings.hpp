#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>
#include <QSize>
#include <QPoint>
#include <QString>

#include "applicationinfo.hpp"

class AppSettings
{
public:
    // Group - MainWindow
    static QSize mainwindow_size;
    static QPoint mainwindow_pos;
    static bool mainwindow_topmost;

    // Group - Main
    static QString main_dialog_file;
    static QString main_dialog_dir;
    static int main_open_dir_level;

    // Group - Viewer
    static int viewer_scaling_mode;
    static qreal viewer_scaling_times;
    static int viewer_ipixmode;

    // Group - Playlist
    static bool playlist_visible;
    static bool playlist_slideshow_repeat;
    static int playlist_slideshow_interval;

    static void SaveSettings();
    static void LoadSettings();

private:
    AppSettings(){}
};

#endif // APPSETTINGS_H
