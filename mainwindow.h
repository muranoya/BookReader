#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "imagemanager.h"
#include "nullptr.h"
#include <iostream>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QImage>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QDir>
#include <QFileDialog>

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

private slots:
    void on_menu_File_Open_triggered();
    void on_menu_File_FolderOpen_triggered();

private:
    Ui::MainWindow *ui;
    ImageManager *imgManager;

    void mousePressEvent(QMouseEvent *event);
    void updateWindowState();
};

#endif // MAINWINDOW_H
