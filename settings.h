#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

namespace BookReader
{
namespace Settings
{
    static QSettings mysettings("settings.ini", QSettings::IniFormat);
}
}
#endif // SETTINGS_H
