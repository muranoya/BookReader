#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->imgManager = new ImageManager(ui->graphicsView);
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
}

void MainWindow::updateWindowState()
{
    QString title = this->imgManager->getShowingFileName();
    if (title.length() == 0)
    {
        this->setWindowTitle(QString(SOFTWARE_NAME));
    }
    else
    this->setWindowTitle(this->imgManager->getShowingFileName());
}

void MainWindow::on_menu_File_Open_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("Imges (*.png *.jpg *.jpeg *.bmp *.gif)"));

    if (!filename.isEmpty())
    {
        this->imgManager->loadFile(filename);
        this->setWindowTitle(this->imgManager->getShowingFileName());
    }
}

void MainWindow::on_menu_File_FolderOpen_triggered()
{
    QString dirname = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QString());

    if (!dirname.isEmpty())
    {
        this->imgManager->loadDir(dirname);
        this->setWindowTitle(this->imgManager->getShowingFileName());
    }
}

void MainWindow::on_menu_File_Close_triggered()
{
    this->imgManager->releaseImages();
    updateWindowState();
}

void MainWindow::on_menu_View_FullSize_triggered()
{

}

void MainWindow::on_menu_View_FitWindow_triggered()
{

}

void MainWindow::on_menu_View_SetScale_triggered()
{
    SettingScaleDialog dialog(this);
    dialog.getScale(100.0);
}

void MainWindow::on_menu_View_AssignScale_triggered()
{
    //ui->graphicsView->scale();
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
