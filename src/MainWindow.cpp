#include <QDir>
#include "MainWindow.hpp"
#include "ScaleDialog.hpp"
#include "AppSettings.hpp"
#include "SettingDialog.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , imgview(new ImageViewer(this))
{
    addDockWidget(Qt::LeftDockWidgetArea, imgview->playlistDock());
    setCentralWidget(imgview);

    connect(imgview->playlistDock(), SIGNAL(visibilityChanged(bool)),
            this, SLOT(imgview_playlistVisibleChanged(bool)));
    connect(imgview, SIGNAL(changedImage()),
            this, SLOT(imgview_changedImage()));
    connect(imgview, SIGNAL(changedScaleMode()),
            this, SLOT(imgview_changedScaleMode()));

    createMenus();

    AppSettings::LoadSettings();
    applySettings();
    updateWindowText();
}

MainWindow::~MainWindow()
{
    delete imgview;

    delete menu_file;
    delete menu_file_open;
    delete menu_file_fopen;
    delete menu_file_settings;
    delete menu_file_aboutqt;
    delete menu_file_exit;

    delete menu_view;
    delete menu_view_fullsize;
    delete menu_view_fitwindow;
    delete menu_view_fitimage;
    delete menu_view_setscale;
    delete menu_view_spread;
    delete menu_view_rightbinding;
    delete menu_view_nn;
    delete menu_view_bi;
    delete menu_view_bc;

    delete menu_window;
    delete menu_window_playlist;
}

/******************* file *******************/
void
MainWindow::menu_file_open_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(
                this, tr("ファイルを開く"), AppSettings::diag_path,
                imgview->readableExtFormat());

    if (!files.isEmpty())
    {
        const QDir dir = QFileInfo(files.at(0)).absoluteDir();
        AppSettings::diag_path = dir.absolutePath();
        imgview->clearPlaylist();
        imgview->openImages(files);
        updateWindowText();
    }
}

void
MainWindow::menu_file_fopen_triggered()
{
    QString dirname = QFileDialog::getExistingDirectory(
                this, tr("ディレクトリを開く"), AppSettings::diag_path);

    if (!dirname.isEmpty())
    {
        AppSettings::diag_path = dirname;
        imgview->clearPlaylist();
        imgview->openImages(QStringList(dirname));
        updateWindowText();
    }
}

void
MainWindow::menu_file_settings_triggered()
{
    if (SettingDialog::openSettingDialog())
    {
        imgview->setOpenDirLevel(AppSettings::viewer_openlevel);
        imgview->setImageCacheSize(AppSettings::pl_prefetch);
    }
}

/******************* view *******************/
void
MainWindow::menu_view_fullsize_triggered()
{
    changeCheckedScaleMenu(menu_view_fullsize, ImageViewer::FULLSIZE);
}

void
MainWindow::menu_view_fitwindow_triggered()
{
    changeCheckedScaleMenu(menu_view_fitwindow, ImageViewer::FIT_WINDOW);
}

void
MainWindow::menu_view_fitimage_triggered()
{
    changeCheckedScaleMenu(menu_view_fitimage, ImageViewer::FIT_IMAGE);
}

void
MainWindow::menu_view_setscale_triggered()
{
    qreal ret;
    if (ScaleDialog::getScale(imgview->imageSize(), imgview->getScale(), ret))
    {
        changeCheckedScaleMenu(menu_view_setscale, ImageViewer::CUSTOM_SCALE, ret);
    }
    else
    {
        menu_view_setscale->setChecked(false);
    }
}

void
MainWindow::menu_view_spread_triggered()
{
    imgview->setSpreadMode(menu_view_spread->isChecked());
}

void
MainWindow::menu_view_rightbinding_triggered()
{
    imgview->setRightbindingMode(menu_view_rightbinding->isChecked());
}

void
MainWindow::menu_view_nn_triggered()
{
    if (menu_view_nn->isChecked())
    {
        menu_view_bi->setChecked(false);
        menu_view_bc->setChecked(false);
        imgview->setInterpolationMode(ImageViewer::NearestNeighbor);
    }
    else
    {
        menu_view_nn->setChecked(true);
        menu_view_bi->setChecked(false);
        menu_view_bc->setChecked(false);
        imgview->setInterpolationMode(ImageViewer::NearestNeighbor);
    }
}

void
MainWindow::menu_view_bi_triggered()
{
    if (menu_view_bi->isChecked())
    {
        menu_view_nn->setChecked(false);
        menu_view_bc->setChecked(false);
        imgview->setInterpolationMode(ImageViewer::Bilinear);
    }
    else
    {
        menu_view_nn->setChecked(false);
        menu_view_bi->setChecked(true);
        menu_view_bc->setChecked(false);
        imgview->setInterpolationMode(ImageViewer::Bilinear);
    }
}

void
MainWindow::menu_view_bc_triggered()
{
    if (menu_view_bc->isChecked())
    {
        menu_view_nn->setChecked(false);
        menu_view_bi->setChecked(false);
        imgview->setInterpolationMode(ImageViewer::Bicubic);
    }
    else
    {
        menu_view_nn->setChecked(false);
        menu_view_bi->setChecked(false);
        menu_view_bc->setChecked(true);
        imgview->setInterpolationMode(ImageViewer::Bicubic);
    }
}

/******************* window *******************/
void
MainWindow::menu_window_playlist_triggered()
{
    imgview->playlistDock()->setVisible(menu_window_playlist->isChecked());
}

/******************* util *******************/
void
MainWindow::updateWindowText()
{
    QString title;
    if (imgview->empty())
    {
        title = tr("%1").arg(AppSettings::SOFTWARE_NAME);
    }
    else
    {
        if (imgview->countShowImages() == 1)
        {
            title = tr("[%1/%2]")
                .arg(imgview->currentIndex(0)+1)
                .arg(imgview->count());
        }
        else
        {
            title = tr("[%1-%2/%3]")
                .arg(imgview->currentIndex(0)+1)
                .arg(imgview->currentIndex(1)+1)
                .arg(imgview->count());
        }

        if (imgview->countShowImages() == 2)
        {
            title = (imgview->isRightbindingMode() ? tr("%1 %3 | %2")
                                                   : tr("%1 %2 | %3"))
                .arg(title)
                .arg(imgview->currentFileName(0))
                .arg(imgview->currentFileName(1));
        }
        else
        {
            title = tr("%1 %2")
                .arg(title)
                .arg(imgview->currentFileName(0));
        }

        title = tr("%1 倍率:%3%")
            .arg(title)
            .arg(QString::number(imgview->getScale() * 100.0, 'g', 4));
    }
    setWindowTitle(title);
}

/******************* image viewer event *******************/
void
MainWindow::imgview_playlistVisibleChanged(bool visible)
{
    menu_window_playlist->setChecked(visible);
}

void
MainWindow::imgview_changedImage()
{
    updateWindowText();
}

void
MainWindow::imgview_changedScaleMode()
{
    updateWindowText();

    menu_view_fullsize->setChecked(false);
    menu_view_fitwindow->setChecked(false);
    menu_view_fitimage->setChecked(false);
    menu_view_setscale->setChecked(false);
    switch (imgview->getScaleMode())
    {
        case ImageViewer::FULLSIZE:
            menu_view_fullsize->setChecked(true);
            break;
        case ImageViewer::FIT_WINDOW:
            menu_view_fitwindow->setChecked(true);
            break;
        case ImageViewer::FIT_IMAGE:
            menu_view_fitimage->setChecked(true);
            break;
        case ImageViewer::CUSTOM_SCALE:
            menu_view_setscale->setChecked(true);
            break;
    }
}

void
MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    storeSettings();
    AppSettings::SaveSettings();
}

void
MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    updateWindowText();
}

/******************* util *******************/
void
MainWindow::createMenus()
{
    menu_file = new QMenu(tr("ファイル"), this);
    menu_file_open     = new QAction(tr("ファイルを開く"), this);
    menu_file_fopen    = new QAction(tr("フォルダを開く"), this);
    menu_file_settings = new QAction(tr("設定"), this);
    menu_file_aboutqt  = new QAction(tr("Qtについて"), this);
    menu_file_exit     = new QAction(tr("終了"), this);

    menu_file->addAction(menu_file_open);
    menu_file->addAction(menu_file_fopen);
    menu_file->addSeparator();
    menu_file->addAction(menu_file_settings);
    menu_file->addSeparator();
    menu_file->addAction(menu_file_aboutqt);
    menu_file->addSeparator();
    menu_file->addAction(menu_file_exit);
    connect(menu_file_open,     SIGNAL(triggered()),
            this, SLOT(menu_file_open_triggered()));
    connect(menu_file_fopen,    SIGNAL(triggered()),
            this, SLOT(menu_file_fopen_triggered()));
    connect(menu_file_settings, SIGNAL(triggered()),
            this, SLOT(menu_file_settings_triggered()));
    connect(menu_file_aboutqt,  SIGNAL(triggered()),
            QCoreApplication::instance(), SLOT(aboutQt()));
    connect(menu_file_exit,     SIGNAL(triggered()),
            QCoreApplication::instance(), SLOT(quit()));
    menuBar()->addMenu(menu_file);


    menu_view = new QMenu(tr("表示"), this);
    menu_view_fullsize     = new QAction(tr("原寸大で表示する"), this);
    menu_view_fullsize->setCheckable(true);
    menu_view_fitwindow    = new QAction(tr("ウィンドウに合わせて表示する"), this);
    menu_view_fitwindow->setCheckable(true);
    menu_view_fitimage     = new QAction(tr("横幅に合わせて表示する"), this);
    menu_view_fitimage->setCheckable(true);
    menu_view_setscale     = new QAction(tr("倍率を指定して表示する"), this);
    menu_view_setscale->setCheckable(true);
    menu_view_spread       = new QAction(tr("見開き表示"), this);
    menu_view_spread->setCheckable(true);
    menu_view_rightbinding = new QAction(tr("右綴じ表示"), this);
    menu_view_rightbinding->setCheckable(true);
    menu_view_nn           = new QAction(tr("低品質(Nearest Neighbor)"), this);
    menu_view_nn->setCheckable(true);
    menu_view_bi           = new QAction(tr("バランス(Bilinear)"), this);
    menu_view_bi->setCheckable(true);
    menu_view_bc           = new QAction(tr("高品質(Bicubic)"), this);
    menu_view_bc->setCheckable(true);

    menu_view->addAction(menu_view_fullsize);
    menu_view->addAction(menu_view_fitwindow);
    menu_view->addAction(menu_view_fitimage);
    menu_view->addAction(menu_view_setscale);
    menu_view->addSeparator();
    menu_view->addAction(menu_view_spread);
    menu_view->addAction(menu_view_rightbinding);
    menu_view->addSeparator();
    menu_view->addAction(menu_view_nn);
    menu_view->addAction(menu_view_bi);
    menu_view->addAction(menu_view_bc);
    connect(menu_view_fullsize,     SIGNAL(triggered()),
            this, SLOT(menu_view_fullsize_triggered()));
    connect(menu_view_fitwindow,    SIGNAL(triggered()),
            this, SLOT(menu_view_fitwindow_triggered()));
    connect(menu_view_fitimage,     SIGNAL(triggered()),
            this, SLOT(menu_view_fitimage_triggered()));
    connect(menu_view_setscale,     SIGNAL(triggered()),
            this, SLOT(menu_view_setscale_triggered()));
    connect(menu_view_spread,       SIGNAL(triggered()),
            this, SLOT(menu_view_spread_triggered()));
    connect(menu_view_rightbinding, SIGNAL(triggered()),
            this, SLOT(menu_view_rightbinding_triggered()));
    connect(menu_view_nn,           SIGNAL(triggered()),
            this, SLOT(menu_view_nn_triggered()));
    connect(menu_view_bi,           SIGNAL(triggered()),
            this, SLOT(menu_view_bi_triggered()));
    connect(menu_view_bc,           SIGNAL(triggered()),
            this, SLOT(menu_view_bc_triggered()));
    menuBar()->addMenu(menu_view);


    menu_window = new QMenu(tr("ウィンドウ"), this);
    menu_window_playlist = new QAction(tr("プレイリスト"), this);
    menu_window_playlist->setCheckable(true);

    menu_window->addAction(menu_window_playlist);
    connect(menu_window_playlist, SIGNAL(triggered()),
            this, SLOT(menu_window_playlist_triggered()));
    menuBar()->addMenu(menu_window);
}

void
MainWindow::changeCheckedScaleMenu(QAction *act,
        const ImageViewer::ViewMode m, const qreal s)
{
    menu_view_fullsize->setChecked(false);
    menu_view_fitwindow->setChecked(false);
    menu_view_fitimage->setChecked(false);
    menu_view_setscale->setChecked(false);

    act->setChecked(true);

    if (m == ImageViewer::CUSTOM_SCALE)
    {
        imgview->setScale(m, s);
    }
    else
    {
        imgview->setScale(m);
    }
    updateWindowText();
}

void
MainWindow::applySettings()
{
    resize(AppSettings::mw_size);
    move(AppSettings::mw_pos);

    ImageViewer::ViewMode mode = 
        ImageViewer::ViewMode(AppSettings::viewer_scalingmode);
    switch (mode)
    {
        case ImageViewer::FULLSIZE:
            changeCheckedScaleMenu(menu_view_fullsize, mode);
            break;
        case ImageViewer::FIT_WINDOW:
            changeCheckedScaleMenu(menu_view_fitwindow, mode);
            break;
        case ImageViewer::FIT_IMAGE:
            changeCheckedScaleMenu(menu_view_fitimage, mode);
            break;
        case ImageViewer::CUSTOM_SCALE:
            changeCheckedScaleMenu(menu_view_setscale, mode,
                    AppSettings::viewer_scalingtimes);
            break;
    }
    
    imgview->setInterpolationMode(
            ImageViewer::InterpolationMode(AppSettings::viewer_ipixmode));
    switch (imgview->getInterpolationMode())
    {
        case ImageViewer::NearestNeighbor:
            menu_view_nn->setChecked(true);
            break;
        case ImageViewer::Bilinear:
            menu_view_bi->setChecked(true);
            break;
        case ImageViewer::Bicubic:
            menu_view_bc->setChecked(true);
            break;
    }

    imgview->setSpreadMode(AppSettings::viewer_spread);
    menu_view_spread->setChecked(AppSettings::viewer_spread);

    imgview->setRightbindingMode(AppSettings::viewer_rightbinding);
    menu_view_rightbinding->setChecked(AppSettings::viewer_rightbinding);
    
    imgview->setOpenDirLevel(AppSettings::viewer_openlevel);

    imgview->playlistDock()->setVisible(AppSettings::pl_visible);
    menu_window_playlist->setChecked(imgview->playlistDock()->isVisible());

    imgview->setImageCacheSize(AppSettings::pl_prefetch);
}

void
MainWindow::storeSettings()
{
    AppSettings::mw_size = size();
    AppSettings::mw_pos = pos();

    AppSettings::viewer_scalingmode = int(imgview->getScaleMode());
    AppSettings::viewer_scalingtimes = imgview->getScale();
    AppSettings::viewer_ipixmode = int(imgview->getInterpolationMode());
    AppSettings::viewer_spread = menu_view_spread->isChecked();
    AppSettings::viewer_rightbinding = menu_view_rightbinding->isChecked();
    AppSettings::viewer_openlevel = imgview->getOpenDirLevel();

    AppSettings::pl_visible = imgview->playlistDock()->isVisible();
    AppSettings::pl_prefetch = imgview->getImageCacheSize();
}

