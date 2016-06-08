#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSize>
#include <QPoint>
#include <QString>

class AppSettings
{
public:
    // Group - MainWindow
    static QSize mainwindow_size;
    static QPoint mainwindow_pos;

    // Group - Main
    static QString main_dialog_file;
    static QString main_dialog_dir;

    // Group - Viewer
    static int viewer_scaling_mode;
    static qreal viewer_scaling_times;
    static int viewer_ipixmode;
    static bool viewer_spread;
    static bool viewer_rightbinding;
    static int viewer_open_dir_level;

    // Group - Playlist
    static bool playlist_visible;
    static int playlist_prefetch;

    static void SaveSettings();
    static void LoadSettings();

private:
    AppSettings(){}
};

#endif // APPSETTINGS_H
