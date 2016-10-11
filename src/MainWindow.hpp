#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ImageViewer.hpp"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMainWindow>
#include <QMouseEvent>
#include <QFileDialog>
#include <QApplication>

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
    void menu_view_fitimage_triggered();
    void menu_view_setscale_triggered();
    void menu_view_spread_triggered();
    void menu_view_rightbinding_triggered();
    void menu_view_nn_triggered();
    void menu_view_bi_triggered();
    void menu_view_bc_triggered();

    /******************* window *******************/
    void menu_window_playlist_triggered();

    /******************* util *******************/
    void updateWindowText();

    /******************* image viewer event *******************/
    void imgview_playlistVisibleChanged(bool visible);
    void imgview_changedImage();
    void imgview_changedScaleMode();

protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    ImageViewer *imgview;

    QMenu *menu_file;
    QAction *menu_file_open;
    QAction *menu_file_fopen;
    QAction *menu_file_settings;
    QAction *menu_file_aboutqt;
    QAction *menu_file_exit;
    QMenu *menu_view;
    QAction *menu_view_fullsize;
    QAction *menu_view_fitwindow;
    QAction *menu_view_fitimage;
    QAction *menu_view_setscale;
    QAction *menu_view_spread;
    QAction *menu_view_rightbinding;
    QAction *menu_view_nn;
    QAction *menu_view_bi;
    QAction *menu_view_bc;
    QMenu *menu_window;
    QAction *menu_window_playlist;

    /******************* util *******************/
    void createMenus();
    void changeCheckedScaleMenu(QAction *act,
            const ImageViewer::ViewMode m, const double s = 0.0);
    void applySettings();
    void storeSettings();
};

#endif // MAINWINDOW_H
