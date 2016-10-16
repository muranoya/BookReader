#include <QSettings>
#include "App.hpp"
#include "Viewer.hpp"

QSize  App::mw_size;
QPoint App::mw_pos;

int    App::view_scalem;
double App::view_scale;
int    App::view_ipix;
bool   App::view_spread;
bool   App::view_rbind;
int    App::view_openlevel;
int    App::view_feedpage;

bool   App::pl_visible;
int    App::pl_prefetch;

const QString App::SOFTWARE_ORG("muranoya.net");
const QString App::SOFTWARE_NAME("SpRead");

void
App::SaveSettings()
{
    QSettings s(SOFTWARE_ORG, SOFTWARE_NAME);

    s.beginGroup("MainWindow");
    s.setValue("size",     mw_size);
    s.setValue("location", mw_pos);
    s.endGroup();

    s.beginGroup("Viewer");
    s.setValue("scalem",    view_scalem);
    s.setValue("scale",     view_scale);
    s.setValue("ipix",      view_ipix);
    s.setValue("spread",    view_spread);
    s.setValue("rbind",     view_rbind);
    s.setValue("openlevel", view_openlevel);
    s.setValue("feedpage",  view_feedpage);
    s.endGroup();

    s.beginGroup("Playlist");
    s.setValue("visible",  pl_visible);
    s.setValue("prefetch", pl_prefetch);
    s.endGroup();
}

void
App::LoadSettings()
{
    QSettings s(SOFTWARE_ORG, SOFTWARE_NAME);

    s.beginGroup("MainWindow");
    mw_size = s.value("size",     QSize(600, 400)).toSize();
    mw_pos  = s.value("location", QPoint(100, 100)).toPoint();
    s.endGroup();

    s.beginGroup("Viewer");
    view_scalem    = s.value("scalem",    Viewer::FittingWindow).toInt();
    view_scale     = s.value("scale",     1.0).toReal();
    view_ipix      = s.value("ipix",      Viewer::Bilinear).toInt();
    view_spread    = s.value("spread",    false).toBool();
    view_rbind     = s.value("rbind",     false).toBool();
    view_openlevel = s.value("openlevel", 99).toInt();
    view_feedpage  = s.value("feedpage",  Viewer::MouseButton).toInt();
    s.endGroup();

    s.beginGroup("Playlist");
    pl_visible  = s.value("visible",  true).toBool();
    pl_prefetch = s.value("prefetch", 20).toInt();
    s.endGroup();
}

