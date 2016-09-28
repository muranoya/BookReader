#include "MainWindow.hpp"
#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    int id = QFontDatabase::addApplicationFont(":/ipagp.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont ipa(family);
    a.setFont(ipa);
    w.show();

    return a.exec();
}
