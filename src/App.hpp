#ifndef APP_HPP
#define APP_HPP

#include <QSize>
#include <QPoint>
#include <QString>

class App
{
public:
    // Group - MainWindow
    static QSize  mw_size;
    static QPoint mw_pos;

    // Group - Viewer
    static int    view_scalem;
    static double view_scale;
    static int    view_ipix;
    static bool   view_spread;
    static bool   view_autospread;
    static bool   view_rbind;
    static int    view_openlevel;
    static int    view_feedpage;

    // Group - Playlist
    static bool pl_visible;
    static int  pl_prefetch;

    static const QString SOFTWARE_ORG;
    static const QString SOFTWARE_NAME;

    static void SaveSettings();
    static void LoadSettings();

private:
    App() = delete;
};

#endif // APP_HPP
