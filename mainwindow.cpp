#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QGraphicsPixmapItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 変数の初期化
    view_scene = nullptr;
    view_img = nullptr;
    view_item = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_menu_File_Open_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QString(),
                                                    tr("Imges (*.png *.jpg *.bmp)"));

    if (!filename.isEmpty())
    {
        QImage *img = new QImage(filename);

        if (img->isNull())
        {
            delete img;
            QMessageBox::critical(this, tr("Error"), tr("画像の読み込みに失敗しました"));
            return;
        }

        QGraphicsScene *scene = new QGraphicsScene();
        ui->graphicsView->setScene(scene);
        QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(*img));
        scene->addItem(item);

        delete view_img;
        delete view_item;
        delete view_scene;

        view_scene = scene;
        view_img = img;
        view_item = item;
    }
}
