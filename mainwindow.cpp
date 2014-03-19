#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    imgView = new ImageViewer();
    ui->gridLayout_2->addWidget(imgView);
    connect(imgView, SIGNAL(imageChanged()), SLOT(updateWindowState()));
    connect(imgView, SIGNAL(sizeChanged()), SLOT(updateWindowState()));

    restoreSettings();
    updateWindowState();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete imgView;
}

/******************* file *******************/
void MainWindow::on_menu_File_Open_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(
                this, tr("ファイルを開く"),
#ifdef __APPLE__
                dialog_File+"/test.txt",
#else
                dialog_File,
#endif
                tr("Images (*png *.jpg *.jpeg *.bmp *.gif"));

    if (files.isEmpty())
    {
        return;
    }

    QFileInfo info(files.at(0));
    QDir dir = info.absoluteDir();
    dialog_File = dir.absolutePath();
    imgView->loadFiles(files);
}

void MainWindow::on_menu_File_FolderOpen_triggered()
{
    QString dirname = QFileDialog::getExistingDirectory(
                this, tr("ディレクトリを開く"), dialog_Directory);

    if (!dirname.isEmpty())
    {
        dialog_Directory = dirname;
        imgView->loadDir(dirname);
    }
}

void MainWindow::on_menu_File_Settings_triggered()
{
    SettingsDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_menu_File_Close_triggered()
{
    imgView->releaseImages();
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
    if (dialog.getScale(imgView->getScale()))
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
    if (imgView->playSlideShow())
    {
        imgView->stopSlideShow();
    }
    else
    {
        imgView->startSlideShow();
    }
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
    QString title = imgView->getFileName();
    if (title.isEmpty())
    {
        setWindowTitle(BookReader::SOFTWARE_NAME);
    }
    else
    {
        title = tr("[%1/%2] %3 [倍率:%4%]")
                .arg(QString::number(imgView->getImageListIndex() + 1))
                .arg(QString::number(imgView->getImageListCount()))
                .arg(title)
                .arg(QString::number(imgView->getScale() * 100.0, 'g', 4));
        setWindowTitle(title);
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
        imgView->setScale(m, s);
    }
    else
    {
        imgView->setScale(m);
    }
    updateWindowState();
}

void MainWindow::saveSettings()
{
    QSettings &settings = BookReader::Settings::mysettings;

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.endGroup();

    settings.beginGroup("Main");
    settings.setValue("dialog_file", dialog_File);
    settings.setValue("dialog_directory", dialog_Directory);
    settings.endGroup();

    settings.beginGroup("Viewer");
    settings.setValue("scaling_mode", imgView->getScaleMode());
    settings.setValue("scaling_times", imgView->getScale());
    //settings.setValue("completion", );
    settings.endGroup();
}

void MainWindow::restoreSettings()
{
    QSettings &settings = BookReader::Settings::mysettings;

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(600, 400)).toSize());
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

    //settings.value("completion", );
    settings.endGroup();
}
