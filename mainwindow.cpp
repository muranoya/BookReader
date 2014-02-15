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
    this->setWindowTitle(this->imgManager->getShowingFileName());
}
