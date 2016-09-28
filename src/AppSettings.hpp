#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSize>
#include <QPoint>
#include <QString>

class AppSettings
{
public:
    // Group - MainWindow
    static QSize mw_size;
    static QPoint mw_pos;

    // Group - Main
    static QString diag_path;

    // Group - Viewer
    static int viewer_scalingmode;
    static double viewer_scalingtimes;
    static int viewer_ipixmode;
    static bool viewer_spread;
    static bool viewer_rightbinding;
    static int viewer_openlevel;

    // Group - Playlist
    static bool pl_visible;
    static int pl_prefetch;

    static const QString SOFTWARE_ORG;
    static const QString SOFTWARE_NAME;

    static void SaveSettings();
    static void LoadSettings();

private:
    AppSettings(){}
};

#endif // APPSETTINGS_H
