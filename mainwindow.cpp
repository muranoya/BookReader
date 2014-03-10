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
    QString filename = QFileDialog::getOpenFileName(
                this, tr("ファイルを開く"), QString(),
                tr("Imges (*.png *.jpg *.jpeg *.bmp *.gif)"));

    if (!filename.isEmpty())
    {
        imgView->loadFile(filename);
    }
}

void MainWindow::on_menu_File_FolderOpen_triggered()
{
    QString dirname = QFileDialog::getExistingDirectory(
                this, tr("ディレクトリを開く"), QString());

    if (!dirname.isEmpty())
    {
        imgView->loadDir(dirname);
    }
}

void MainWindow::on_menu_File_Close_triggered()
{
    imgView->releaseImages();
}

/******************* view *******************/
void MainWindow::on_menu_View_FullSize_triggered()
{
    changeCheckedScaleMenu(ui->menu_View_FullSize, ImageViewer::FULLSIZE, 0.0);
}

void MainWindow::on_menu_View_FitWindow_triggered()
{
    changeCheckedScaleMenu(ui->menu_View_FitWindow, ImageViewer::FIT_WINDOW, 0.0);
}

void MainWindow::on_menu_View_FitImage_triggered()
{
    changeCheckedScaleMenu(ui->menu_View_FitImage, ImageViewer::FIT_IMAGE, 0.0);
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

void MainWindow::on_menu_View_Rotate90_triggered()
{
    changeCheckedRotateMenu(ui->menu_View_Rotate90, 90.0);
}

void MainWindow::on_menu_View_Rotate180_triggered()
{
    changeCheckedRotateMenu(ui->menu_View_Rotate180, 180.0);
}

void MainWindow::on_menu_View_Rotate270_triggered()
{
    changeCheckedRotateMenu(ui->menu_View_Rotate270, 270.0);
}

void MainWindow::on_menu_View_SetRotate_triggered()
{
    if (ui->menu_View_SetRotate->isChecked())
    {
        SettingRotateDialog dialog(this);
        if (dialog.getRotate(imgView->getRotate()))
        {
            changeCheckedRotateMenu(ui->menu_View_SetRotate, dialog.getValue());
        }
        else
        {
            ui->menu_View_SetRotate->setChecked(false);
        }
    }
    else
    {
        changeCheckedRotateMenu(ui->menu_View_SetRotate, 0.0);
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
        title = tr("[%1/%2] %3 [倍率:%4% 回転:%5度]")
                .arg(QString::number(imgView->getImageListIndex() + 1))
                .arg(QString::number(imgView->getImageListCount()))
                .arg(title)
                .arg(QString::number(imgView->getScale() * 100.0, 'g', 4))
                .arg(QString::number(imgView->getRotate(), 'g', 4));
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

void MainWindow::changeCheckedRotateMenu(QAction *act, qreal deg)
{
    bool b = act->isChecked();
    ui->menu_View_Rotate90->setChecked(false);
    ui->menu_View_Rotate180->setChecked(false);
    ui->menu_View_Rotate270->setChecked(false);
    ui->menu_View_SetRotate->setChecked(false);

    act->setChecked(b);
    if (b)
    {
        imgView->setRotate(deg);
    }
    else
    {
        imgView->setRotate(0.0);
    }
    updateWindowState();
}
