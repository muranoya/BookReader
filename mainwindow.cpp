#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->imgManager = new ImageManager(ui->graphicsView);
    updateWindowState();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete imgManager;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        this->imgManager->nextImage();
    }
    if (event->buttons() & Qt::RightButton)
    {
        this->imgManager->previousImage();
    }
    updateWindowState();
    setupMatrix();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    setupMatrix();
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        ui->menu_View_FullScreen->setChecked(this->isFullScreen());
    }
}

void MainWindow::updateWindowState()
{
    QString title = this->imgManager->getShowingFileName();
    if (title.length() == 0)
    {
        this->setWindowTitle(QString(SOFTWARE_NAME));
    }
    else
    {
        this->setWindowTitle(this->imgManager->getShowingFileName());
    }
}

void MainWindow::setupMatrix()
{
    QMatrix matrix;

    if (ui->menu_View_FitWindow->isChecked())
    {
        qreal ws = 1.0, hs = 1.0, s;
        if (ui->graphicsView->size().width() < imgManager->getShowingImageSize().width())
        {
            ws = (qreal)ui->graphicsView->size().width() / (qreal)imgManager->getShowingImageSize().width();
        }
        if (ui->graphicsView->size().height() < imgManager->getShowingImageSize().height())
        {
            hs = (qreal)ui->graphicsView->size().height() / (qreal)imgManager->getShowingImageSize().height();
        }
        s = ws > hs ? hs : ws;
        matrix.scale(s, s);
    }

    ui->graphicsView->setMatrix(matrix);
}

void MainWindow::on_menu_File_Open_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("Imges (*.png *.jpg *.jpeg *.bmp *.gif)"));

    if (!filename.isEmpty())
    {
        this->imgManager->loadFile(filename);
        updateWindowState();
    }
}

void MainWindow::on_menu_File_FolderOpen_triggered()
{
    QString dirname = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QString());

    if (!dirname.isEmpty())
    {
        this->imgManager->loadDir(dirname);
        updateWindowState();
    }
}

void MainWindow::on_menu_File_Close_triggered()
{
    this->imgManager->releaseImages();
    updateWindowState();
}

void MainWindow::on_menu_View_FullSize_triggered()
{
    if (ui->menu_View_FullSize->isChecked())
    {
        ui->menu_View_FullSize->setChecked(true);

        ui->menu_View_FitWindow->setChecked(false);
        ui->menu_View_AssignScale->setChecked(false);
    }
    else
    {
        ui->menu_View_FullSize->setChecked(true);
    }
    setupMatrix();
}

void MainWindow::on_menu_View_FitWindow_triggered()
{
    if (ui->menu_View_FitWindow->isChecked())
    {
        ui->menu_View_FitWindow->setChecked(true);

        ui->menu_View_FullSize->setChecked(false);
        ui->menu_View_AssignScale->setChecked(false);
    }
    else
    {
        ui->menu_View_FitWindow->setChecked(true);
    }
    setupMatrix();
}

void MainWindow::on_menu_View_SetScale_triggered()
{
    SettingScaleDialog dialog(this);
    dialog.getScale(100.0);
}

void MainWindow::on_menu_View_AssignScale_triggered()
{
    if (ui->menu_View_AssignScale->isChecked())
    {
        ui->menu_View_AssignScale->setChecked(true);

        ui->menu_View_FullSize->setChecked(false);
        ui->menu_View_FitWindow->setChecked(false);
    }
    else
    {
        ui->menu_View_AssignScale->setChecked(true);
    }
    setupMatrix();
}

void MainWindow::on_menu_View_FullScreen_triggered()
{
    if (ui->menu_View_FullScreen->isChecked())
    {
        this->showFullScreen();
    }
    else
    {
        this->showNormal();
    }
}

void MainWindow::on_menu_Help_Version_triggered()
{
    VersionDialog dialog(this);
    dialog.showVersion();
}
