#include "mainwindow.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , imgView(new ImageViewer(this))
    , pldock(new PlaylistDock(this))
    , histdialog(new HistgramDialog())
{
    addDockWidget(Qt::LeftDockWidgetArea, pldock);
    setCentralWidget(imgView);

    connect(pldock, SIGNAL(itemOpen(QString)),         this, SLOT(playlistItemOpened(QString)));
    connect(pldock, SIGNAL(itemRemoved(bool)),         this, SLOT(playlistItemRemoved(bool)));
    connect(pldock, SIGNAL(visibilityChanged(bool)),   this, SLOT(playlistVisibleChanged(bool)));
    connect(pldock, SIGNAL(slideshow_stop()),          this, SLOT(playlistSlideshowStop()));
    connect(pldock, SIGNAL(slideshow_change(QString)), this, SLOT(playlistSlideshowChange(QString)));

    connect(imgView, SIGNAL(rightClicked()),              this, SLOT(viewerRightClicked()));
    connect(imgView, SIGNAL(leftClicked()),               this, SLOT(viewerLeftClicked()));
    connect(imgView, SIGNAL(dropItems(QStringList,bool)), this, SLOT(viewerDropItems(QStringList,bool)));
    connect(imgView, SIGNAL(setNewImage()),               this, SLOT(viewerSetNewImage()));

    connect(histdialog, SIGNAL(closeDialog()), this, SLOT(closeHistgramDialog()));

    createMenus();

    AppSettings::LoadSettings();
    applySettings();
    updateWindowText();
}

MainWindow::~MainWindow()
{
    delete imgView;
    delete pldock;

    delete menu_file;
    delete menu_file_open;
    delete menu_file_fopen;
    delete menu_file_settings;

    delete menu_view;
    delete menu_view_fullsize;
    delete menu_view_fitwindow;
    delete menu_view_fitimage;
    delete menu_view_setscale;
    delete menu_view_slideshow;
    delete menu_view_fullscreen;
    delete menu_view_filter;

    delete menu_window;
    delete menu_window_alwaystop;
    delete menu_window_hide;
    delete menu_window_playlist;
    delete menu_window_histgram;

    delete menu_help;
    delete menu_help_benchmark;
    delete menu_help_aboutqt;
    delete menu_help_version;
}

/******************* file *******************/
void
MainWindow::menu_file_open_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(
                this, tr("ファイルを開く"), AppSettings::main_dialog_file,
                tr("Images (*.png *.jpg *.jpeg *.bmp *.gif)"));

    if (!files.isEmpty())
    {
        const QFileInfo info(files.at(0));
        const QDir dir = info.absoluteDir();
        AppSettings::main_dialog_file = dir.absolutePath();
        pldock->clear();
        pldock->append(files);
        imgView->showImage(pldock->currentFilePath());
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
        pldock->clear();
        pldock->append(QStringList(dirname), AppSettings::main_open_dir_level);
        imgView->showImage(pldock->currentFilePath());
        updateWindowText();
    }
}

void
MainWindow::menu_file_settings_triggered()
{
    SettingsDialog dialog(this);
    dialog.exec();

    imgView->setInterpolationMode(ImageViewer::InterpolationMode(AppSettings::viewer_ipixmode));
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
    if (dialog.getScale(imgView->getOriginalImageSize(), imgView->getScale()))
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
MainWindow::menu_view_slideshow_triggered()
{
    if (pldock->isPlayingSlideshow())
    {
        pldock->stopSlideshow();
    }
    else
    {
        pldock->setSlideshowRepeat(AppSettings::playlist_slideshow_repeat);
        pldock->setSlideshowInterval(int(AppSettings::playlist_slideshow_interval*1000));
        pldock->startSlideshow();
    }
    updateWindowText();
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

/******************* window *******************/
void
MainWindow::menu_window_alwaystop_triggered()
{
    setWindowTopMost(menu_window_alwaystop->isChecked());
}

void
MainWindow::menu_window_hide_triggered()
{
    setWindowState(Qt::WindowMinimized);
}

void
MainWindow::menu_window_playlist_triggered()
{
    pldock->setVisible(!pldock->isVisible());
}

void
MainWindow::menu_window_histgram_triggered()
{
    if (menu_window_histgram->isChecked())
    {
        histdialog->setHistgram(imgView->histgram());
    }
    else
    {
        histdialog->close();
    }
}

/******************* help *******************/
void
MainWindow::menu_help_benchmark_triggered()
{
    BenchmarkDialog dialog(this);
    dialog.exec();
}

void
MainWindow::menu_help_aboutqt_triggered()
{
    QApplication::aboutQt();
}

void
MainWindow::menu_help_version_triggered()
{
    VersionDialog *dialog = new VersionDialog(this);
    dialog->show();
}

/******************* util *******************/
void
MainWindow::updateWindowText()
{
    QString title = pldock->currentFileName();
    if (title.isEmpty())
    {
        setWindowTitle(BookReader::SOFTWARE_NAME);
    }
    else
    {
        title = tr("[%1/%2] %3 [倍率:%4%]")
                .arg(QString::number(pldock->currentIndex() + 1))
                .arg(QString::number(pldock->count()))
                .arg(title)
                .arg(QString::number(imgView->getScale() * 100.0, 'g', 4));

        if (pldock->isPlayingSlideshow())
        {
            title = tr("%1 [スライドショー]")
                    .arg(title);
        }
        setWindowTitle(title);
    }
}

/******************* playlist event *******************/
void
MainWindow::playlistVisibleChanged(bool visible)
{
    menu_window_playlist->setChecked(visible);
}

void
MainWindow::playlistItemRemoved(bool currentFile)
{
    if (pldock->empty())
    {
        imgView->releaseImage();
    }
    else if (currentFile && !pldock->empty())
    {
        imgView->showImage(pldock->currentFilePath());
    }

    if (currentFile && histdialog->isVisible())
    {
        histdialog->releaseHistgram();

        if (!pldock->empty())
        {
            histdialog->setHistgram(imgView->histgram());
        }
    }
    updateWindowText();
}

void
MainWindow::playlistItemOpened(QString path)
{
    imgView->showImage(path);
    updateWindowText();
}

void
MainWindow::playlistSlideshowStop()
{
    updateWindowText();
    menu_view_slideshow->setChecked(false);
}

void
MainWindow::playlistSlideshowChange(QString name)
{
    imgView->showImage(name);
    updateWindowText();
}

/******************* image viewer event *******************/
void
MainWindow::viewerRightClicked()
{
    if (pldock->empty())
    {
        return;
    }
    imgView->showImage(pldock->previousFilePath());
    updateWindowText();
}

void
MainWindow::viewerLeftClicked()
{
    if (pldock->empty())
    {
        return;
    }
    imgView->showImage(pldock->nextFilePath());
    updateWindowText();
}

void
MainWindow::viewerDropItems(QStringList list, bool copy)
{
    const bool isempty = pldock->empty();
    if (!copy)
    {
        pldock->clear();
    }

    pldock->append(list, AppSettings::main_open_dir_level);

    if (!copy || isempty)
    {
        imgView->showImage(pldock->currentFilePath());
    }
    updateWindowText();
}

void
MainWindow::viewerSetNewImage()
{
    if (histdialog->isVisible())
    {
        histdialog->releaseHistgram();
        if (!imgView->empty())
        {
            histdialog->setHistgram(imgView->histgram());
        }
    }
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
    menu_file->addAction(menu_file_open);
    menu_file->addAction(menu_file_fopen);
    menu_file->addSeparator();
    menu_file->addAction(menu_file_settings);
    connect(menu_file_open, SIGNAL(triggered()), this, SLOT(menu_file_open_triggered()));
    connect(menu_file_fopen, SIGNAL(triggered()), this, SLOT(menu_file_fopen_triggered()));
    connect(menu_file_settings, SIGNAL(triggered()), this, SLOT(menu_file_settings_triggered()));
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
    menu_view->addAction(menu_view_slideshow);
    menu_view->addSeparator();
    menu_view->addAction(menu_view_fullscreen);
    menu_view->addSeparator();
    menu_view->addAction(menu_view_filter);
    connect(menu_view_fullsize, SIGNAL(triggered()), this, SLOT(menu_view_fullsize_triggered()));
    connect(menu_view_fitwindow, SIGNAL(triggered()), this, SLOT(menu_view_fitwindow_triggered()));
    connect(menu_view_fitimage, SIGNAL(triggered()), this, SLOT(menu_view_fitimage_triggered()));
    connect(menu_view_setscale, SIGNAL(triggered()), this, SLOT(menu_view_setscale_triggered()));
    connect(menu_view_slideshow, SIGNAL(triggered()), this, SLOT(menu_view_slideshow_triggered()));
    connect(menu_view_fullscreen, SIGNAL(triggered()), this, SLOT(menu_view_fullscreen_triggered()));
    menuBar()->addMenu(menu_view);

    menu_window = new QMenu(this);
    menu_window->setTitle(tr("ウィンドウ"));
    menu_window_alwaystop = new QAction(tr("常に手前に表示する"), this);
    menu_window_alwaystop->setCheckable(true);
    menu_window_hide = new QAction(tr("最小化"), this);
    menu_window_hide->setShortcut(tr("Ctrl+M"));
    menu_window_playlist = new QAction(tr("プレイリスト"), this);
    menu_window_playlist->setCheckable(true);
    menu_window_histgram = new QAction(tr("ヒストグラム"), this);
    menu_window_histgram->setCheckable(true);
    menu_window->addAction(menu_window_alwaystop);
    menu_window->addAction(menu_window_hide);
    menu_window->addSeparator();
    menu_window->addAction(menu_window_playlist);
    menu_window->addAction(menu_window_histgram);
    connect(menu_window_alwaystop, SIGNAL(triggered()), this, SLOT(menu_window_alwaystop_triggered()));
    connect(menu_window_hide, SIGNAL(triggered()), this, SLOT(menu_window_hide_triggered()));
    connect(menu_window_playlist, SIGNAL(triggered()), this, SLOT(menu_window_playlist_triggered()));
    connect(menu_window_histgram, SIGNAL(triggered()), this, SLOT(menu_window_histgram_triggered()));
    menuBar()->addMenu(menu_window);

    menu_help = new QMenu(this);
    menu_help->setTitle(tr("ヘルプ"));
    menu_help_benchmark = new QAction(tr("ベンチマーク"), this);
    menu_help_aboutqt = new QAction(tr("Qtについて"), this);
    menu_help_version = new QAction(tr("バージョン"), this);
    menu_help->addAction(menu_help_benchmark);
    menu_help->addSeparator();
    menu_help->addAction(menu_help_aboutqt);
    menu_help->addAction(menu_help_version);
    connect(menu_help_benchmark, SIGNAL(triggered()), this, SLOT(menu_help_benchmark_triggered()));
    connect(menu_help_aboutqt, SIGNAL(triggered()), this, SLOT(menu_help_aboutqt_triggered()));
    connect(menu_help_version, SIGNAL(triggered()), this, SLOT(menu_help_version_triggered()));
    menuBar()->addMenu(menu_help);
}

void
MainWindow::changeCheckedScaleMenu(QAction *act, const ImageViewer::ViewMode m, const qreal s)
{
    menu_view_fullsize->setChecked(false);
    menu_view_fitwindow->setChecked(false);
    menu_view_fitimage->setChecked(false);
    menu_view_setscale->setChecked(false);

    act->setChecked(true);

    if (m == ImageViewer::CUSTOM_SCALE)
    {
        imgView->setScale(m, s);
    }
    else
    {
        imgView->setScale(m);
    }
    updateWindowText();
}

void
MainWindow::setWindowTopMost(bool flag)
{
    setWindowFlags(flag ? (windowFlags() | Qt::WindowStaysOnTopHint)
                        : (windowFlags() & (!Qt::WindowStaysOnTopHint)));
    show();
}

bool
MainWindow::getWindowTopMost()
{
    return bool(windowFlags() & Qt::WindowStaysOnTopHint);
}

void
MainWindow::applySettings()
{
    resize(AppSettings::mainwindow_size);
    move(AppSettings::mainwindow_pos);
    setWindowTopMost(AppSettings::mainwindow_topmost);
    menu_window_alwaystop->setChecked(AppSettings::mainwindow_topmost);

    ImageViewer::ViewMode mode = ImageViewer::ViewMode(AppSettings::viewer_scaling_mode);
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
        changeCheckedScaleMenu(menu_view_setscale, mode, AppSettings::viewer_scaling_times);
        break;
    }
    imgView->setInterpolationMode(ImageViewer::InterpolationMode(AppSettings::viewer_ipixmode));

    pldock->setVisible(AppSettings::playlist_visible);
    menu_window_playlist->setChecked(pldock->isVisible());
}

void
MainWindow::storeSettings()
{
    AppSettings::mainwindow_size = size();
    AppSettings::mainwindow_pos = pos();
    AppSettings::mainwindow_topmost = getWindowTopMost();

    AppSettings::viewer_scaling_mode = int(imgView->getScaleMode());
    AppSettings::viewer_scaling_times = imgView->getScale();
    AppSettings::viewer_ipixmode = int(imgView->getInterpolationMode());

    AppSettings::playlist_visible = pldock->isVisible();
}
