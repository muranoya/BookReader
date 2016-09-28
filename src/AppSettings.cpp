#include <QSettings>
#include "AppSettings.hpp"

QSize AppSettings::mw_size;
QPoint AppSettings::mw_pos;

QString AppSettings::diag_path;

int AppSettings::viewer_scalingmode;
double AppSettings::viewer_scalingtimes;
int AppSettings::viewer_ipixmode;
bool AppSettings::viewer_spread;
bool AppSettings::viewer_rightbinding;
int AppSettings::viewer_openlevel;

const QString AppSettings::SOFTWARE_ORG("muranoya.net");
const QString AppSettings::SOFTWARE_NAME("BookReader");

bool AppSettings::pl_visible;
int AppSettings::pl_prefetch;

void
AppSettings::SaveSettings()
{
    QSettings settings(SOFTWARE_ORG, SOFTWARE_NAME);

    settings.beginGroup("MainWindow");
    settings.setValue("size", mw_size);
    settings.setValue("location", mw_pos);
    settings.endGroup();

    settings.beginGroup("Main");
    settings.setValue("dialog_path", diag_path);
    settings.endGroup();

    settings.beginGroup("Viewer");
    settings.setValue("scaling_mode", viewer_scalingmode);
    settings.setValue("scaling_times", viewer_scalingtimes);
    settings.setValue("ipix_mode", viewer_ipixmode);
    settings.setValue("image_spread", viewer_spread);
    settings.setValue("rightbinding", viewer_rightbinding);
    settings.setValue("open_level", viewer_openlevel);
    settings.endGroup();

    settings.beginGroup("Playlist");
    settings.setValue("visible", pl_visible);
    settings.setValue("prefetch", pl_prefetch);
    settings.endGroup();
}

void
AppSettings::LoadSettings()
{
    QSettings settings(SOFTWARE_ORG, SOFTWARE_NAME);

    settings.beginGroup("MainWindow");
    mw_size = settings.value("size", QSize(600, 400)).toSize();
    mw_pos = settings.value("location", QPoint(100, 100)).toPoint();
    settings.endGroup();

    settings.beginGroup("Main");
    diag_path = settings.value("dialog_path", QString()).toString();
    settings.endGroup();

    settings.beginGroup("Viewer");
    viewer_scalingmode = settings.value("scaling_mode", 1).toInt();
    viewer_scalingtimes = settings.value("scaling_times", 1.0).toReal();
    viewer_ipixmode = settings.value("ipix_mode", 1).toInt();
    viewer_spread = settings.value("image_spread", false).toBool();
    viewer_rightbinding = settings.value("rightbinding", false).toBool();
    viewer_openlevel = settings.value("open_level", 999).toInt();
    settings.endGroup();

    settings.beginGroup("Playlist");
    pl_visible = settings.value("visible", true).toBool();
    pl_prefetch = settings.value("prefetch", 20).toInt();
    settings.endGroup();
}

