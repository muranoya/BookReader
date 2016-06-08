#include <QSettings>
#include "AppSettings.hpp"
#include "appinfo.hpp"

QSize AppSettings::mainwindow_size;
QPoint AppSettings::mainwindow_pos;

QString AppSettings::main_dialog_file;
QString AppSettings::main_dialog_dir;

int AppSettings::viewer_scaling_mode;
qreal AppSettings::viewer_scaling_times;
int AppSettings::viewer_ipixmode;
bool AppSettings::viewer_spread;
bool AppSettings::viewer_rightbinding;
int AppSettings::viewer_open_dir_level;

bool AppSettings::playlist_visible;
int AppSettings::playlist_prefetch;

void
AppSettings::SaveSettings()
{
    QSettings settings(BookReader::SOFTWARE_ORGANIZATION,
            BookReader::SOFTWARE_NAME);

    settings.beginGroup("MainWindow");
    settings.setValue("size", mainwindow_size);
    settings.setValue("location", mainwindow_pos);
    settings.endGroup();

    settings.beginGroup("Main");
    settings.setValue("dialog_file", main_dialog_file);
    settings.setValue("dialog_directory", main_dialog_dir);
    settings.endGroup();

    settings.beginGroup("Viewer");
    settings.setValue("scaling_mode", viewer_scaling_mode);
    settings.setValue("scaling_times", viewer_scaling_times);
    settings.setValue("ipix_mode", viewer_ipixmode);
    settings.setValue("image_spread", viewer_spread);
    settings.setValue("rightbinding", viewer_rightbinding);
    settings.setValue("open_dir_level", viewer_open_dir_level);
    settings.endGroup();

    settings.beginGroup("Playlist");
    settings.setValue("visible", playlist_visible);
    settings.setValue("prefetch", playlist_prefetch);
    settings.endGroup();
}

void
AppSettings::LoadSettings()
{
    QSettings settings(BookReader::SOFTWARE_ORGANIZATION,
            BookReader::SOFTWARE_NAME);

    settings.beginGroup("MainWindow");
    mainwindow_size = settings.value("size", QSize(600, 400)).toSize();
    mainwindow_pos = settings.value("location", QPoint(100, 100)).toPoint();
    settings.endGroup();

    settings.beginGroup("Main");
    main_dialog_file = settings.value("dialog_file", QString()).toString();
    main_dialog_dir = settings.value("location", QString()).toString();
    settings.endGroup();

    settings.beginGroup("Viewer");
    viewer_scaling_mode = settings.value("scaling_mode", 1).toInt();
    viewer_scaling_times = settings.value("scaling_times", 1.0).toReal();
    viewer_ipixmode = settings.value("ipix_mode", 1).toInt();
    viewer_spread = settings.value("image_spread", false).toBool();
    viewer_rightbinding = settings.value("rightbinding", false).toBool();
    viewer_open_dir_level = settings.value("open_dir_level", 999).toInt();
    settings.endGroup();

    settings.beginGroup("Playlist");
    playlist_visible = settings.value("visible", true).toBool();
    playlist_prefetch = settings.value("prefetch", 20).toInt();
    settings.endGroup();
}

