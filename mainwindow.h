#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QImage>
#include <QGraphicsPixmapItem>

const class nullptr_t
{
public:
    template<class T>
    operator T*() const { return 0; }

    template<class C, class T>
    operator T C::*() const { return 0; }

private:
    void operator&() const;
} nullptr = {};

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    QGraphicsScene *view_scene;
    QImage *view_img;
    QGraphicsPixmapItem *view_item;

private slots:
    void on_menu_File_Open_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
