#include <QDir>
#include "mainwindow.hpp"
#include "settingscaledialog.hpp"
#include "appinfo.hpp"
#include "appsettings.hpp"
#include "settingsdialog.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , imgview(new ImageViewer(this))
    , histdialog(new HistgramDialog())
{
    addDockWidget(Qt::LeftDockWidgetArea, imgview->playlistDock());
    setCentralWidget(imgview);

    connect(imgview->playlistDock(), SIGNAL(visibilityChanged(bool)),
            this, SLOT(imgview_playlistVisibleChanged(bool)));
    connect(imgview, SIGNAL(stoppedSlideshow()),
            this, SLOT(imgview_stoppedSlideshow()));
    connect(imgview, SIGNAL(changeImage()),
            this, SLOT(imgview_changeImage()));

    connect(histdialog, SIGNAL(closeDialog()),
            this, SLOT(closeHistgramDialog()));

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
    delete menu_file_exit;

    delete menu_view;
    delete menu_view_fullsize;
    delete menu_view_fitwindow;
    delete menu_view_fitimage;
    delete menu_view_setscale;
    delete menu_view_spread;
    delete menu_view_rightbinding;
    delete menu_view_slideshow;
    delete menu_view_fullscreen;
    delete menu_view_filter;

    delete menu_imgproc;
    delete menu_imgproc_nn;
    delete menu_imgproc_bi;
    delete menu_imgproc_bc;

    delete menu_window;
    delete menu_window_hide;
    delete menu_window_playlist;
    delete menu_window_histgram;

    delete menu_help;
    delete menu_help_aboutqt;
}

/******************* file *******************/
void
MainWindow::menu_file_open_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(
                this, tr("ファイルを開く"), AppSettings::main_dialog_file,
                "Images (*.png *.jpg *.jpeg *.bmp *.gif)");

    if (!files.isEmpty())
    {
        const QFileInfo info(files.at(0));
        const QDir dir = info.absoluteDir();
        AppSettings::main_dialog_file = dir.absolutePath();
        imgview->clearPlaylist();
        imgview->openImages(files);
        updateWindowText();
    }
}

void
MainWindow::menu_file_fopen_triggered()
{
    QString dirname = QFileDialog::getExistingDirectory(
                this, tr("ディレクトリを開く"), AppSettings::main_dialog_dir);

    if (!dirname.isEmpty())
    {
        AppSettings::main_dialog_dir = dirname;
        imgview->clearPlaylist();
        imgview->openImages(QStringList(dirname));
        updateWindowText();
    }
}

void
MainWindow::menu_file_settings_triggered()
{
    SettingsDialog dialog(this);
    dialog.exec();

    imgview->setOpenDirLevel(AppSettings::viewer_open_dir_level);
    imgview->setSlideshowInterval(AppSettings::playlist_slideshow_interval);
}

void
MainWindow::menu_file_exit_triggered()
{
    QApplication::quit();
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
    SettingScaleDialog dialog(this);
    if (dialog.getScale(imgview->combinedImageSize(), imgview->getScale()))
    {
        changeCheckedScaleMenu(menu_view_setscale, ImageViewer::CUSTOM_SCALE,
                               dialog.getValue());
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
MainWindow::menu_view_slideshow_triggered()
{
    if (imgview->isPlayingSlideshow())
    {
        imgview->stopSlideshow();
    }
    else
    {
        imgview->startSlideshow();
        updateWindowText();
    }
}

void
MainWindow::menu_view_fullscreen_triggered()
{
    if (menu_view_fullscreen->isChecked())
    {
        showFullScreen();
    }
    else
    {
        showNormal();
    }
}

/******************* image processing *******************/
void
MainWindow::menu_imgproc_nn_triggered()
{
    if (menu_imgproc_nn->isChecked())
    {
        menu_imgproc_bi->setChecked(false);
        menu_imgproc_bc->setChecked(false);
        imgview->setInterpolationMode(ImageViewer::NearestNeighbor);
    }
    else
    {
        menu_imgproc_nn->setChecked(true);
        menu_imgproc_bi->setChecked(false);
        menu_imgproc_bc->setChecked(false);
        imgview->setInterpolationMode(ImageViewer::NearestNeighbor);
    }
}

void
MainWindow::menu_imgproc_bi_triggered()
{
    if (menu_imgproc_bi->isChecked())
    {
        menu_imgproc_nn->setChecked(false);
        menu_imgproc_bc->setChecked(false);
        imgview->setInterpolationMode(ImageViewer::Bilinear);
    }
    else
    {
        menu_imgproc_nn->setChecked(false);
        menu_imgproc_bi->setChecked(true);
        menu_imgproc_bc->setChecked(false);
        imgview->setInterpolationMode(ImageViewer::Bilinear);
    }
}

void
MainWindow::menu_imgproc_bc_triggered()
{
    if (menu_imgproc_bc->isChecked())
    {
        menu_imgproc_nn->setChecked(false);
        menu_imgproc_bi->setChecked(false);
        imgview->setInterpolationMode(ImageViewer::Bicubic);
    }
    else
    {
        menu_imgproc_nn->setChecked(false);
        menu_imgproc_bi->setChecked(false);
        menu_imgproc_bc->setChecked(true);
        imgview->setInterpolationMode(ImageViewer::Bicubic);
    }
}

/******************* window *******************/
void
MainWindow::menu_window_hide_triggered()
{
    setWindowState(Qt::WindowMinimized);
}

void
MainWindow::menu_window_playlist_triggered()
{
    imgview->playlistDock()->setVisible(menu_window_playlist->isChecked());
}

void
MainWindow::menu_window_histgram_triggered()
{
    if (menu_window_histgram->isChecked())
    {
        histdialog->setHistgram(imgview->histgram());
    }
    else
    {
        histdialog->close();
    }
}

/******************* help *******************/
void
MainWindow::menu_help_aboutqt_triggered()
{
    QApplication::aboutQt();
}

/******************* util *******************/
void
MainWindow::updateWindowText()
{
    QString title;
    if (imgview->empty())
    {
        title = tr("%1 %2")
            .arg(BookReader::SOFTWARE_NAME)
            .arg(BookReader::SOFTWARE_VERSION);
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

        QSize s;
        s = imgview->orgImageSize(0);
        QString img1title = tr("%1 [%2, %3]")
            .arg(imgview->currentFileName(0))
            .arg(s.width())
            .arg(s.height());

        if (imgview->countShowImages() == 2)
        {
            s = imgview->orgImageSize(1);
            QString img2title = tr("%1 [%2, %3]")
                .arg(imgview->currentFileName(1))
                .arg(s.width())
                .arg(s.height());

            title = (imgview->isRightbindingMode() ? tr("%1 %3 | %2")
                                                   : tr("%1 %2 | %3"))
                .arg(title)
                .arg(img1title)
                .arg(img2title);
        }
        else
        {
            title = tr("%1 %2")
                .arg(title)
                .arg(img1title);
        }

        title = tr("%1 倍率:%3%")
            .arg(title)
            .arg(QString::number(imgview->getScale() * 100.0, 'g', 4));

        if (imgview->isPlayingSlideshow())
        {
            title = tr("%1 [スライドショー]")
                .arg(title);
        }
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
MainWindow::imgview_stoppedSlideshow()
{
    menu_view_slideshow->setChecked(false);
    updateWindowText();
}

void
MainWindow::imgview_changeImage()
{
    if (histdialog->isVisible())
    {
        histdialog->releaseHistgram();
        if (!imgview->empty())
        {
            histdialog->setHistgram(imgview->histgram());
        }
    }
    updateWindowText();
}

void
MainWindow::closeHistgramDialog()
{
    menu_window_histgram->setChecked(false);
}

void
MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        menu_view_fullscreen->setChecked(isFullScreen());
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
    menu_file = new QMenu(this);
    menu_file->setTitle(tr("ファイル"));
    menu_file_open = new QAction(tr("ファイルを開く"), this);
    menu_file_open->setShortcut(tr("Ctrl+O"));
    menu_file_fopen = new QAction(tr("フォルダを開く"), this);
    menu_file_fopen->setShortcut(tr("Ctrl+D"));
    menu_file_settings = new QAction(tr("設定画面を開く"), this);
    menu_file_exit = new QAction(tr("終了"), this);
    menu_file_exit->setShortcut(tr("Ctrl+Q"));
    menu_file->addAction(menu_file_open);
    menu_file->addAction(menu_file_fopen);
    menu_file->addSeparator();
    menu_file->addAction(menu_file_settings);
    menu_file->addSeparator();
    menu_file->addAction(menu_file_exit);
    connect(menu_file_open, SIGNAL(triggered()),
            this, SLOT(menu_file_open_triggered()));
    connect(menu_file_fopen, SIGNAL(triggered()),
            this, SLOT(menu_file_fopen_triggered()));
    connect(menu_file_settings, SIGNAL(triggered()),
            this, SLOT(menu_file_settings_triggered()));
    connect(menu_file_exit, SIGNAL(triggered()),
            this, SLOT(menu_file_exit_triggered()));
    menuBar()->addMenu(menu_file);

    menu_view = new QMenu(this);
    menu_view->setTitle(tr("表示"));
    menu_view_fullsize = new QAction(tr("原寸大で表示する"), this);
    menu_view_fullsize->setCheckable(true);
    menu_view_fitwindow = new QAction(tr("ウィンドウに合わせて表示する"), this);
    menu_view_fitwindow->setCheckable(true);
    menu_view_fitimage = new QAction(tr("画像に合わせて表示する"), this);
    menu_view_fitimage->setCheckable(true);
    menu_view_setscale = new QAction(tr("倍率を指定して表示する"), this);
    menu_view_setscale->setCheckable(true);
    menu_view_spread = new QAction(tr("見開き表示"), this);
    menu_view_spread->setCheckable(true);
    menu_view_rightbinding = new QAction(tr("右綴じ表示"), this);
    menu_view_rightbinding->setCheckable(true);
    menu_view_slideshow = new QAction(tr("スライドショー"), this);
    menu_view_slideshow->setShortcut(tr("Ctrl+Shift+F"));
    menu_view_slideshow->setCheckable(true);
    menu_view_fullscreen = new QAction(tr("フルスクリーン"), this);
    menu_view_fullscreen->setCheckable(true);
    menu_view_fullscreen->setShortcut(tr("Ctrl+F"));
    menu_view_filter = new QAction(tr("フィルター"), this);
    menu_view->addAction(menu_view_fullsize);
    menu_view->addAction(menu_view_fitwindow);
    menu_view->addAction(menu_view_fitimage);
    menu_view->addAction(menu_view_setscale);
    menu_view->addSeparator();
    menu_view->addAction(menu_view_spread);
    menu_view->addAction(menu_view_rightbinding);
    menu_view->addSeparator();
    menu_view->addAction(menu_view_slideshow);
    menu_view->addSeparator();
    menu_view->addAction(menu_view_fullscreen);
    menu_view->addSeparator();
    menu_view->addAction(menu_view_filter);
    connect(menu_view_fullsize, SIGNAL(triggered()),
            this, SLOT(menu_view_fullsize_triggered()));
    connect(menu_view_fitwindow, SIGNAL(triggered()),
            this, SLOT(menu_view_fitwindow_triggered()));
    connect(menu_view_fitimage, SIGNAL(triggered()),
            this, SLOT(menu_view_fitimage_triggered()));
    connect(menu_view_setscale, SIGNAL(triggered()),
            this, SLOT(menu_view_setscale_triggered()));
    connect(menu_view_spread, SIGNAL(triggered()),
            this, SLOT(menu_view_spread_triggered()));
    connect(menu_view_rightbinding, SIGNAL(triggered()),
            this, SLOT(menu_view_rightbinding_triggered()));
    connect(menu_view_slideshow, SIGNAL(triggered()),
            this, SLOT(menu_view_slideshow_triggered()));
    connect(menu_view_fullscreen, SIGNAL(triggered()),
            this, SLOT(menu_view_fullscreen_triggered()));
    menuBar()->addMenu(menu_view);

    menu_imgproc = new QMenu(this);
    menu_imgproc->setTitle(tr("画像処理"));
    menu_imgproc_nn = new QAction(tr("Nearest Neighbor"), this);
    menu_imgproc_nn->setCheckable(true);
    menu_imgproc_bi = new QAction(tr("Bilinear"), this);
    menu_imgproc_bi->setCheckable(true);
    menu_imgproc_bc = new QAction(tr("Bicubic"), this);
    menu_imgproc_bc->setCheckable(true);
    menu_imgproc->addAction(menu_imgproc_nn);
    menu_imgproc->addAction(menu_imgproc_bi);
    menu_imgproc->addAction(menu_imgproc_bc);
    connect(menu_imgproc_nn, SIGNAL(triggered()),
            this, SLOT(menu_imgproc_nn_triggered()));
    connect(menu_imgproc_bi, SIGNAL(triggered()),
            this, SLOT(menu_imgproc_bi_triggered()));
    connect(menu_imgproc_bc, SIGNAL(triggered()),
            this, SLOT(menu_imgproc_bc_triggered()));
    menuBar()->addMenu(menu_imgproc);

    menu_window = new QMenu(this);
    menu_window->setTitle(tr("ウィンドウ"));
    menu_window_hide = new QAction(tr("最小化"), this);
    menu_window_hide->setShortcut(tr("Ctrl+M"));
    menu_window_playlist = new QAction(tr("プレイリスト"), this);
    menu_window_playlist->setCheckable(true);
    menu_window_histgram = new QAction(tr("ヒストグラム"), this);
    menu_window_histgram->setCheckable(true);
    menu_window->addAction(menu_window_hide);
    menu_window->addSeparator();
    menu_window->addAction(menu_window_playlist);
    menu_window->addAction(menu_window_histgram);
    connect(menu_window_hide, SIGNAL(triggered()),
            this, SLOT(menu_window_hide_triggered()));
    connect(menu_window_playlist, SIGNAL(triggered()),
            this, SLOT(menu_window_playlist_triggered()));
    connect(menu_window_histgram, SIGNAL(triggered()),
            this, SLOT(menu_window_histgram_triggered()));
    menuBar()->addMenu(menu_window);

    menu_help = new QMenu(this);
    menu_help->setTitle(tr("ヘルプ"));
    menu_help_aboutqt = new QAction(tr("Qtについて"), this);
    menu_help->addAction(menu_help_aboutqt);
    connect(menu_help_aboutqt, SIGNAL(triggered()),
            this, SLOT(menu_help_aboutqt_triggered()));
    menuBar()->addMenu(menu_help);
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
    resize(AppSettings::mainwindow_size);
    move(AppSettings::mainwindow_pos);

    ImageViewer::ViewMode mode = 
        ImageViewer::ViewMode(AppSettings::viewer_scaling_mode);
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
                    AppSettings::viewer_scaling_times);
            break;
    }
    
    imgview->setInterpolationMode(
            ImageViewer::InterpolationMode(AppSettings::viewer_ipixmode));
    switch (imgview->getInterpolationMode())
    {
        case ImageViewer::NearestNeighbor:
            menu_imgproc_nn->setChecked(true);
            break;
        case ImageViewer::Bilinear:
            menu_imgproc_bi->setChecked(true);
            break;
        case ImageViewer::Bicubic:
            menu_imgproc_bc->setChecked(true);
            break;
    }

    imgview->setSpreadMode(AppSettings::viewer_spread);
    menu_view_spread->setChecked(AppSettings::viewer_spread);

    imgview->setRightbindingMode(AppSettings::viewer_rightbinding);
    menu_view_rightbinding->setChecked(AppSettings::viewer_rightbinding);
    
    imgview->setOpenDirLevel(AppSettings::viewer_open_dir_level);

    imgview->playlistDock()->setVisible(AppSettings::playlist_visible);
    menu_window_playlist->setChecked(imgview->playlistDock()->isVisible());

    imgview->setSlideshowInterval(AppSettings::playlist_slideshow_interval);
}

void
MainWindow::storeSettings()
{
    AppSettings::mainwindow_size = size();
    AppSettings::mainwindow_pos = pos();

    AppSettings::viewer_scaling_mode = int(imgview->getScaleMode());
    AppSettings::viewer_scaling_times = imgview->getScale();
    AppSettings::viewer_ipixmode = int(imgview->getInterpolationMode());
    AppSettings::viewer_spread = menu_view_spread->isChecked();
    AppSettings::viewer_rightbinding = menu_view_rightbinding->isChecked();
    AppSettings::viewer_open_dir_level = imgview->getOpenDirLevel();

    AppSettings::playlist_visible = imgview->playlistDock()->isVisible();
    AppSettings::playlist_slideshow_interval = imgview->getSlideshowInterval();
}

