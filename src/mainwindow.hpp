#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "versiondialog.hpp"
#include "settingscaledialog.hpp"
#include "imageviewer.hpp"
#include "applicationinfo.hpp"
#include "settingsdialog.hpp"
#include "playlistdock.hpp"
#include "histgramdialog.hpp"
#include "benchmarkdialog.hpp"
#include "appsettings.hpp"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMainWindow>
#include <QMouseEvent>
#include <QDir>
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
    void menu_file_exit_triggered();

    /******************* view *******************/
    void menu_view_fullsize_triggered();
    void menu_view_fitwindow_triggered();
    void menu_view_fitimage_triggered();
    void menu_view_setscale_triggered();
    void menu_view_slideshow_triggered();
    void menu_view_fullscreen_triggered();

    /******************* window *******************/
    void menu_window_hide_triggered();
    void menu_window_playlist_triggered();
    void menu_window_histgram_triggered();

    /******************* help *******************/
    void menu_help_benchmark_triggered();
    void menu_help_aboutqt_triggered();
    void menu_help_version_triggered();

    /******************* util *******************/
    void updateWindowText();

    /******************* playlist event *******************/
    void playlistVisibleChanged(bool visible);
    void playlistItemRemoved(bool currentFile);
    void playlistItemOpened(QStringList path);
    void playlistSlideshowStop();
    void playlistSlideshowChange(QStringList name);

    /******************* image viewer event *******************/
    void viewerRightClicked();
    void viewerLeftClicked();
    void viewerDropItems(QStringList list, bool copy);
    void viewerSetNewImage();

    void closeHistgramDialog();

protected:
    virtual void changeEvent(QEvent *event);
    virtual void closeEvent(QCloseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    ImageViewer *imgView;
    PlaylistDock *pldock;

    QMenu *menu_file;
    QAction *menu_file_open;
    QAction *menu_file_fopen;
    QAction *menu_file_settings;
    QAction *menu_file_exit;
    QMenu *menu_view;
    QAction *menu_view_fullsize;
    QAction *menu_view_fitwindow;
    QAction *menu_view_fitimage;
    QAction *menu_view_setscale;
    QAction *menu_view_slideshow;
    QAction *menu_view_fullscreen;
    QAction *menu_view_filter;
    QMenu *menu_window;
    QAction *menu_window_hide;
    QAction *menu_window_playlist;
    QAction *menu_window_histgram;
    QMenu *menu_help;
    QAction *menu_help_benchmark;
    QAction *menu_help_aboutqt;
    QAction *menu_help_version;

    HistgramDialog *histdialog;

    /******************* util *******************/
    void createMenus();
    void changeCheckedScaleMenu(QAction *act,
            const ImageViewer::ViewMode m, const qreal s = 0.0);
    void applySettings();
    void storeSettings();
};

#endif // MAINWINDOW_H
