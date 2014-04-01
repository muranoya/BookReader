#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "versiondialog.h"
#include "settingscaledialog.h"
#include "imageviewer.h"
#include "nullptr.h"
#include "applicationinfo.h"
#include "settingsdialog.h"
#include "playlistdock.h"

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QDir>
#include <QFileDialog>
#include <QTimer>
#include <QSettings>

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
    /******************* file *******************/
    void on_menu_File_Open_triggered();
    void on_menu_File_FolderOpen_triggered();
    void on_menu_File_Settings_triggered();

    /******************* view *******************/
    void on_menu_View_FullSize_triggered();
    void on_menu_View_FitWindow_triggered();
    void on_menu_View_FitImage_triggered();
    void on_menu_View_SetScale_triggered();
    void on_menu_View_FullScreen_triggered();

    /******************* slideshow *******************/
    void on_menu_Slideshow_Slideshow_triggered();

    /******************* window *******************/
    void on_menu_Window_Hide_triggered();
    void on_menu_Window_PlayList_triggered();

    /******************* help *******************/
    void on_menu_Help_Version_triggered();

    /******************* util *******************/
    void updateWindowState();

    /******************* playlist event *******************/
    void playlistVisibleChanged(bool visible);
    void playlistItemRemoved(bool currentFile);
    void playlistItemOpened(QString path);

    /******************* image viewer event *******************/
    void viewerRightClicked();
    void viewerLeftClicked();
    void viewerDropItems(QStringList list, bool copy);

    void slideshow_Timer();

private:
    Ui::MainWindow *ui;
    ImageViewer imgView;
    PlaylistDock pldock;
    QString dialog_File;
    QString dialog_Directory;
    QTimer slideshow;

    /******************* event *******************/
    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);

    /******************* util *******************/
    void changeCheckedScaleMenu(QAction *act, ImageViewer::ViewMode m, qreal s = 0.0);
    void saveSettings();
    void restoreSettings();
};

#endif // MAINWINDOW_H
