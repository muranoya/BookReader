#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "versiondialog.h"
#include "settingscaledialog.h"
#include "imageviewer.h"
#include "nullptr.h"
#include "applicationinfo.h"
#include "settings.h"
#include "settingsdialog.h"

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QDir>
#include <QFileDialog>
#include <QMatrix>
#include <QByteArray>

#include <QTimer>

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
    void on_menu_File_Close_triggered();

    /******************* view *******************/
    void on_menu_View_FullSize_triggered();
    void on_menu_View_FitWindow_triggered();
    void on_menu_View_FitImage_triggered();
    void on_menu_View_SetScale_triggered();
    void on_menu_View_FullScreen_triggered();

    /******************* slideshow *******************/
    void on_menu_Slideshow_Slideshow_triggered();

    /******************* help *******************/
    void on_menu_Help_Version_triggered();

    /******************* util *******************/
    void updateWindowState();

private:
    Ui::MainWindow *ui;
    ImageViewer *imgView;
    QString dialog_File;
    QString dialog_Directory;

    /******************* event *******************/
    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);

    /******************* util *******************/
    void changeCheckedScaleMenu(QAction *act, ImageViewer::ViewMode m, qreal s = 0.0);
    void saveSettings();
    void restoreSettings();
};

#endif // MAINWINDOW_H
