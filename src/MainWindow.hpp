#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMainWindow>
#include <QMouseEvent>
#include <QFileDialog>
#include <QApplication>
#include <QListView>
#include <QDockWidget>
#include "ImageViewer.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    /******************* file *******************/
    void menu_file_open_triggered();
    void menu_file_fopen_triggered();
    void menu_file_settings_triggered();

    /******************* view *******************/
    void menu_view_fullsize_triggered();
    void menu_view_fitwindow_triggered();
    void menu_view_fitwidth_triggered();
    void menu_view_setscale_triggered();
    void menu_view_spread_triggered();
    void menu_view_autospread_triggered();
    void menu_view_rightbinding_triggered();
    void menu_view_nn_triggered();
    void menu_view_bi_triggered();
    void menu_view_bc_triggered();

    /******************* util *******************/
    void updateWindowText();

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    ImageViewer *viewer;
    QListView *plview;
    QDockWidget *dockwidget;

    QAction *pl_show;
    QAction *pl_sep1;
    QAction *pl_remove;
    QAction *pl_clear;

    QMenu *menu_file;
    QAction *menu_file_open;
    QAction *menu_file_fopen;
    QAction *menu_file_settings;
    QAction *menu_file_aboutqt;
    QAction *menu_file_exit;
    QMenu *menu_view;
    QAction *menu_view_fullsize;
    QAction *menu_view_fitwindow;
    QAction *menu_view_fitwidth;
    QAction *menu_view_setscale;
    QAction *menu_view_spread;
    QAction *menu_view_autospread;
    QAction *menu_view_rightbinding;
    QAction *menu_view_nn;
    QAction *menu_view_bi;
    QAction *menu_view_bc;
    QMenu *menu_window;

    QString lastdir;

    /******************* util *******************/
    void createMenus();
    void changeCheckedScaleMenu(QAction *act,
            Viewer::ViewMode m, double s = 0.0);
    void applySettings();
    void storeSettings();
};

#endif // MAINWINDOW_HPP
