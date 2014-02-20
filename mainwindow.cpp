#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    imgManager = new ImageManager(ui->graphicsView);
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
        imgManager->nextImage();
    }
    if (event->buttons() & Qt::RightButton)
    {
        imgManager->previousImage();
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
        ui->menu_View_FullScreen->setChecked(isFullScreen());
    }
}

void MainWindow::changeCheckedScaleMenu(QAction *act)
{
    ui->menu_View_FullSize->setChecked(false);
    ui->menu_View_FitWindow->setChecked(false);
    ui->menu_View_FitImage->setChecked(false);
    ui->menu_View_SetScale->setChecked(false);

    act->setChecked(true);

    setupMatrix();
}

void MainWindow::changeCheckedRotateMenu(QAction *act)
{
    bool b = act->isChecked();
    ui->menu_View_Rotate90->setChecked(false);
    ui->menu_View_Rotate180->setChecked(false);
    ui->menu_View_Rotate270->setChecked(false);
    ui->menu_View_SetRotate->setChecked(false);

    act->setChecked(b);

    setupMatrix();
}

void MainWindow::updateWindowState()
{
    QString title = imgManager->getShowingFileName();
    if (title.length() == 0)
    {
        setWindowTitle(QString(SOFTWARE_NAME));
    }
    else
    {
        setWindowTitle(imgManager->getShowingFileName());
    }
}

void MainWindow::setupMatrix()
{
    QMatrix matrix;

    if (!ui->menu_View_FullSize->isChecked())
    {
        qreal ws = 1.0, hs = 1.0, s = 1.0;
        if (ui->graphicsView->size().width() < imgManager->getShowingImageSize().width())
        {
            ws = (qreal)ui->graphicsView->size().width() / (qreal)imgManager->getShowingImageSize().width();
        }
        if (ui->graphicsView->size().height() < imgManager->getShowingImageSize().height())
        {
            hs = (qreal)ui->graphicsView->size().height() / (qreal)imgManager->getShowingImageSize().height();
        }

        if (ui->menu_View_FitWindow->isChecked())
        {
            s = ws > hs ? hs : ws;
        }
        else if (ui->menu_View_FitImage->isChecked())
        {
            s = ws;
        }

        matrix.scale(s, s);
    }

    if (ui->menu_View_Rotate90->isChecked())
    {
        matrix.rotate(90.0);
    }
    else if (ui->menu_View_Rotate180->isChecked())
    {
        matrix.rotate(180.0);
    }
    else if (ui->menu_View_Rotate270->isChecked())
    {
        matrix.rotate(270.0);
    }
    else if (ui->menu_View_SetRotate->isChecked())
    {
        //matrix.rotate();
    }

    ui->graphicsView->setMatrix(matrix);
}

void MainWindow::on_menu_File_Open_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("ファイルを開く"), QString(),
                                                    tr("Imges (*.png *.jpg *.jpeg *.bmp *.gif)"));

    if (!filename.isEmpty())
    {
        imgManager->loadFile(filename);
        updateWindowState();
    }
}

void MainWindow::on_menu_File_FolderOpen_triggered()
{
    QString dirname = QFileDialog::getExistingDirectory(this, tr("ディレクトリを開く"), QString());

    if (!dirname.isEmpty())
    {
        imgManager->loadDir(dirname);
        updateWindowState();
    }
}

void MainWindow::on_menu_File_Close_triggered()
{
    imgManager->releaseImages();
    updateWindowState();
}

void MainWindow::on_menu_View_ScrollHand_triggered()
{
    ui->graphicsView->setDragMode(ui->menu_View_ScrollHand->isChecked()
                                  ? QGraphicsView::ScrollHandDrag
                                  : QGraphicsView::NoDrag);
    ui->graphicsView->setInteractive(!ui->menu_View_ScrollHand->isChecked());
}

void MainWindow::on_menu_View_FullSize_triggered()
{
    changeCheckedScaleMenu(ui->menu_View_FullSize);
}

void MainWindow::on_menu_View_FitWindow_triggered()
{
    changeCheckedScaleMenu(ui->menu_View_FitWindow);
}

void MainWindow::on_menu_View_FitImage_triggered()
{
    changeCheckedScaleMenu(ui->menu_View_FitImage);
}

void MainWindow::on_menu_View_SetScale_triggered()
{
    SettingScaleDialog dialog(this);
    dialog.getScale(100.0);
    changeCheckedScaleMenu(ui->menu_View_SetScale);
}

void MainWindow::on_menu_View_Rotate90_triggered()
{
    changeCheckedRotateMenu(ui->menu_View_Rotate90);
}

void MainWindow::on_menu_View_Rotate180_triggered()
{
    changeCheckedRotateMenu(ui->menu_View_Rotate180);
}

void MainWindow::on_menu_View_Rotate270_triggered()
{
    changeCheckedRotateMenu(ui->menu_View_Rotate270);
}

void MainWindow::on_menu_View_SetRotate_triggered()
{
    if (ui->menu_View_SetRotate->isChecked())
    {
        SettingRotateDialog dialog(this);
        dialog.getRotate(0.0);
    }
    changeCheckedRotateMenu(ui->menu_View_SetRotate);
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

void MainWindow::on_menu_Filter_Antialiasing_triggered()
{
    ui->graphicsView->setRenderHint(QPainter::Antialiasing, ui->menu_Filter_Antialiasing->isChecked());
}

void MainWindow::on_menu_Help_Version_triggered()
{
    VersionDialog dialog(this);
    dialog.showVersion();
}
