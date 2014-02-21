#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "versiondialog.h"
#include "settingscaledialog.h"
#include "settingrotatedialog.h"
#include "imageviewer.h"
#include "nullptr.h"
#include "ApplicationInfo.h"
#include <iostream>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QImage>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QDir>
#include <QFileDialog>
#include <QMatrix>
#include <QEvent>

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
    void on_menu_File_Close_triggered();

    void on_menu_View_ScrollHand_triggered();
    void on_menu_View_FullSize_triggered();
    void on_menu_View_FitWindow_triggered();
    void on_menu_View_FitImage_triggered();
    void on_menu_View_SetScale_triggered();
    void on_menu_View_Rotate90_triggered();
    void on_menu_View_Rotate180_triggered();
    void on_menu_View_Rotate270_triggered();
    void on_menu_View_SetRotate_triggered();
    void on_menu_View_FullScreen_triggered();

    void on_menu_Filter_Antialiasing_triggered();

    void on_menu_Help_Version_triggered();

private:
    Ui::MainWindow *ui;
    ImageViewer *imgView;

    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void changeEvent(QEvent *event);

    void changeCheckedScaleMenu(QAction *act, ImageViewer::ViewMode m, qreal s);
    void changeCheckedRotateMenu(QAction *act, qreal deg);
    void updateWindowState();
};

#endif // MAINWINDOW_H
