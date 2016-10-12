#include "MainWindow.hpp"
#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
#ifdef USE_CUSTOM_FONT
    int id = QFontDatabase::addApplicationFont(":/ipagp.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont ipa(family);
    a.setFont(ipa);
#endif
    w.show();

    return a.exec();
}
