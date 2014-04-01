#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    imgView(), pldock(this),
    slideshow()
{
    ui->setupUi(this);
    ui->gridLayout_2->addWidget(&imgView);

    addDockWidget(Qt::LeftDockWidgetArea, &pldock);
    connect(&pldock, SIGNAL(itemOpen(QString)), this, SLOT(playlistItemOpened(QString)));
    connect(&pldock, SIGNAL(itemRemoved(bool)), this, SLOT(playlistItemRemoved(bool)));
    connect(&pldock, SIGNAL(visibilityChanged(bool)), this, SLOT(playlistVisibleChanged(bool)));

    connect(&imgView, SIGNAL(rightClicked()), this, SLOT(viewerRightClicked()));
    connect(&imgView, SIGNAL(leftClicked()), this, SLOT(viewerLeftClicked()));
    connect(&imgView, SIGNAL(dropItems(QStringList,bool)), this, SLOT(viewerDropItems(QStringList,bool)));

    connect(&slideshow, SIGNAL(timeout()), this, SLOT(slideshow_Timer()));

    restoreSettings();
    updateWindowState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/******************* file *******************/
void MainWindow::on_menu_File_Open_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(
                this, tr("ファイルを開く"), dialog_File,
                tr("Images (*.png *.jpg *.jpeg *.bmp *.gif)"));

    if (!files.isEmpty())
    {
        QFileInfo info(files.at(0));
        QDir dir = info.absoluteDir();
        dialog_File = dir.absolutePath();
        pldock.clear();
        pldock.append(files);
        imgView.showImage(pldock.currentFilePath());
        updateWindowState();
    }
}

void MainWindow::on_menu_File_FolderOpen_triggered()
{
    QString dirname = QFileDialog::getExistingDirectory(
                this, tr("ディレクトリを開く"), dialog_Directory);

    if (!dirname.isEmpty())
    {
        dialog_Directory = dirname;
        pldock.clear();
        pldock.append(QStringList(dirname), 1);
        imgView.showImage(pldock.currentFilePath());
        updateWindowState();
    }
}

void MainWindow::on_menu_File_Settings_triggered()
{
    SettingsDialog dialog(this);
    dialog.exec();
}

/******************* view *******************/
void MainWindow::on_menu_View_FullSize_triggered()
{
    changeCheckedScaleMenu(ui->menu_View_FullSize, ImageViewer::FULLSIZE);
}

void MainWindow::on_menu_View_FitWindow_triggered()
{
    changeCheckedScaleMenu(ui->menu_View_FitWindow, ImageViewer::FIT_WINDOW);
}

void MainWindow::on_menu_View_FitImage_triggered()
{
    changeCheckedScaleMenu(ui->menu_View_FitImage, ImageViewer::FIT_IMAGE);
}

void MainWindow::on_menu_View_SetScale_triggered()
{
    SettingScaleDialog dialog(this);
    if (dialog.getScale(imgView.getScale()))
    {
        changeCheckedScaleMenu(ui->menu_View_SetScale, ImageViewer::CUSTOM_SCALE,
                               dialog.getValue());
    }
    else
    {
        ui->menu_View_SetScale->setChecked(false);
    }
}

void MainWindow::on_menu_View_FullScreen_triggered()
{
    if (ui->menu_View_FullScreen->isChecked())
    {
        showFullScreen();
    }
    else
    {
        showNormal();
    }
}

/******************* slideshow *******************/
void MainWindow::on_menu_Slideshow_Slideshow_triggered()
{
    if (slideshow.isActive())
    {
        slideshow.stop();
    }
    else
    {
        slideshow.start(3000);
    }
}

/******************* window *******************/
void MainWindow::on_menu_Window_Hide_triggered()
{
    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::on_menu_Window_PlayList_triggered()
{
    pldock.setVisible(!pldock.isVisible());
}

/******************* help *******************/
void MainWindow::on_menu_Help_Version_triggered()
{
    VersionDialog dialog(this);
    dialog.showVersion();
}

/******************* util *******************/
void MainWindow::updateWindowState()
{
    QString title = pldock.currentFileName();
    if (title.isEmpty())
    {
        setWindowTitle(BookReader::SOFTWARE_NAME);
    }
    else
    {
        title = tr("[%1/%2] %3 [倍率:%4%]")
                .arg(QString::number(pldock.currentIndex() + 1))
                .arg(QString::number(pldock.count()))
                .arg(title)
                .arg(QString::number(imgView.getScale() * 100.0, 'g', 4));
        setWindowTitle(title);
    }
}

/******************* playlist event *******************/
void MainWindow::playlistVisibleChanged(bool visible)
{
    ui->menu_Window_PlayList->setChecked(visible);
}

void MainWindow::playlistItemRemoved(bool currentFile)
{
    if (pldock.empty())
    {
        imgView.releaseImage();
    }
    else if (currentFile && !pldock.empty())
    {
        imgView.showImage(pldock.currentFilePath());
    }
    updateWindowState();
}

void MainWindow::playlistItemOpened(QString path)
{
    imgView.showImage(path);
    updateWindowState();
}

/******************* image viewer event *******************/
void MainWindow::viewerRightClicked()
{
    if (pldock.empty())
    {
        return;
    }
    imgView.showImage(pldock.previousFilePath());
    updateWindowState();
}

void MainWindow::viewerLeftClicked()
{
    if (pldock.empty())
    {
        return;
    }
    imgView.showImage(pldock.nextFilePath());
    updateWindowState();
}

void MainWindow::viewerDropItems(QStringList list, bool copy)
{
    bool isempty = pldock.empty();
    if (!copy)
    {
        pldock.clear();
    }

    pldock.append(list, 1);

    if (!copy || isempty)
    {
        imgView.showImage(pldock.currentFilePath());
    }
    updateWindowState();
}

void MainWindow::slideshow_Timer()
{
    if (pldock.empty())
    {
        slideshow.stop();
        ui->menu_Slideshow_Slideshow->setChecked(false);
    }
    else
    {
        imgView.showImage(pldock.nextFilePath());
    }
}

/******************* event *******************/
void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        ui->menu_View_FullScreen->setChecked(isFullScreen());
    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    saveSettings();
}

/******************* util *******************/
void MainWindow::changeCheckedScaleMenu(QAction *act, ImageViewer::ViewMode m, qreal s)
{
    ui->menu_View_FullSize->setChecked(false);
    ui->menu_View_FitWindow->setChecked(false);
    ui->menu_View_FitImage->setChecked(false);
    ui->menu_View_SetScale->setChecked(false);

    act->setChecked(true);

    if (m == ImageViewer::CUSTOM_SCALE)
    {
        imgView.setScale(m, s);
    }
    else
    {
        imgView.setScale(m);
    }
    updateWindowState();
}

void MainWindow::saveSettings()
{
    QSettings settings(BookReader::SOFTWARE_ORGANIZATION, BookReader::SOFTWARE_NAME, this);

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("location", pos());
    settings.endGroup();

    settings.beginGroup("Main");
    settings.setValue("dialog_file", dialog_File);
    settings.setValue("dialog_directory", dialog_Directory);
    settings.endGroup();

    settings.beginGroup("Viewer");
    settings.setValue("scaling_mode", imgView.getScaleMode());
    settings.setValue("scaling_times", imgView.getScale());
    settings.endGroup();

    settings.beginGroup("Playlist");
    settings.setValue("visible", pldock.isVisible());
    settings.endGroup();
}

void MainWindow::restoreSettings()
{
    QSettings settings(BookReader::SOFTWARE_ORGANIZATION, BookReader::SOFTWARE_NAME, this);

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(600, 400)).toSize());
    move(settings.value("location", QPoint(100, 100)).toPoint());
    settings.endGroup();

    settings.beginGroup("Main");
    dialog_File = settings.value("dialog_file", QString()).toString();
    dialog_Directory = settings.value("dialog_directory", QString()).toString();
    settings.endGroup();

    settings.beginGroup("Viewer");
    ImageViewer::ViewMode mode =
            ImageViewer::ViewMode(settings.value("scaling_mode", ImageViewer::FULLSIZE).toInt());
    switch (mode)
    {
    case ImageViewer::FULLSIZE:
        changeCheckedScaleMenu(ui->menu_View_FullSize, mode);
        break;
    case ImageViewer::FIT_WINDOW:
        changeCheckedScaleMenu(ui->menu_View_FitWindow, mode);
        break;
    case ImageViewer::FIT_IMAGE:
        changeCheckedScaleMenu(ui->menu_View_FitImage, mode);
        break;
    case ImageViewer::CUSTOM_SCALE:
        changeCheckedScaleMenu(ui->menu_View_SetScale, mode,
                               settings.value("scaling_times", 0.0).toFloat());
        break;
    }
    settings.endGroup();

    settings.beginGroup("Playlist");
    pldock.setVisible(settings.value("visible", false).toBool());
    ui->menu_Window_PlayList->setChecked(pldock.isVisible());
    settings.endGroup();
}
