#include <QSettings>
#include "appsettings.hpp"
#include "applicationinfo.hpp"

QSize AppSettings::mainwindow_size;
QPoint AppSettings::mainwindow_pos;

QString AppSettings::main_dialog_file;
QString AppSettings::main_dialog_dir;
int AppSettings::main_open_dir_level;

int AppSettings::viewer_scaling_mode;
qreal AppSettings::viewer_scaling_times;
int AppSettings::viewer_ipixmode;
bool AppSettings::viewer_spread;

bool AppSettings::playlist_visible;
int AppSettings::playlist_slideshow_interval;

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
    settings.setValue("open_dir_level", main_open_dir_level);
    settings.endGroup();

    settings.beginGroup("Viewer");
    settings.setValue("scaling_mode", viewer_scaling_mode);
    settings.setValue("scaling_times", viewer_scaling_times);
    settings.setValue("ipix_mode", viewer_ipixmode);
    settings.setValue("image_spread", viewer_spread);
    settings.endGroup();

    settings.beginGroup("Playlist");
    settings.setValue("visible", playlist_visible);
    settings.setValue("slideshow_interval", playlist_slideshow_interval);
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
    main_open_dir_level = settings.value("open_dir_level", 999).toInt();
    settings.endGroup();

    settings.beginGroup("Viewer");
    viewer_scaling_mode = settings.value("scaling_mode", 0).toInt();
    viewer_scaling_times = settings.value("scaling_times", 1.0).toReal();
    viewer_ipixmode = settings.value("ipix_mode", 0).toInt();
    viewer_spread = settings.value("image_spread", false).toBool();
    settings.endGroup();

    settings.beginGroup("Playlist");
    playlist_visible = settings.value("visible", false).toBool();
    playlist_slideshow_interval = settings.value("slideshow_interval", 3000).toInt();
    settings.endGroup();
}
