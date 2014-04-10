#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      imgView(new ImageViewer(this)),
      pldock(new PlaylistDock(this)),
      histdialog(new HistgramDialog()),
      slideshow()
{
    addDockWidget(Qt::LeftDockWidgetArea, pldock);
    setCentralWidget(imgView);

    connect(pldock, SIGNAL(itemOpen(QString)), this, SLOT(playlistItemOpened(QString)));
    connect(pldock, SIGNAL(itemRemoved(bool)), this, SLOT(playlistItemRemoved(bool)));
    connect(pldock, SIGNAL(visibilityChanged(bool)), this, SLOT(playlistVisibleChanged(bool)));

    connect(imgView, SIGNAL(rightClicked()), this, SLOT(viewerRightClicked()));
    connect(imgView, SIGNAL(leftClicked()), this, SLOT(viewerLeftClicked()));
    connect(imgView, SIGNAL(dropItems(QStringList,bool)), this, SLOT(viewerDropItems(QStringList,bool)));
    connect(imgView, SIGNAL(setNewImage()), this, SLOT(viewerSetNewImage()));

    connect(&slideshow, SIGNAL(timeout()), this, SLOT(slideshow_Timer()));

    createMenus();

    restoreSettings();
    updateWindowState();
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
    delete menu_view_fullscreen;
    delete menu_view_filter;

    delete menu_slideshow;
    delete menu_slideshow_slideshow;

    delete menu_window;
    delete menu_window_hide;
    delete menu_window_playlist;
    delete menu_window_histgram;
    delete menu_window_torncurve;

    delete menu_help;
    delete menu_help_version;
}

/******************* file *******************/
void MainWindow::menu_file_open_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(
                this, tr("ファイルを開く"), dialog_File,
                tr("Images (*.png *.jpg *.jpeg *.bmp *.gif)"));

    if (!files.isEmpty())
    {
        QFileInfo info(files.at(0));
        QDir dir = info.absoluteDir();
        dialog_File = dir.absolutePath();
        pldock->clear();
        pldock->append(files);
        imgView->showImage(pldock->currentFilePath());
        updateWindowState();
    }
}

void MainWindow::menu_file_fopen_triggered()
{
    QString dirname = QFileDialog::getExistingDirectory(
                this, tr("ディレクトリを開く"), dialog_Directory);

    if (!dirname.isEmpty())
    {
        dialog_Directory = dirname;
        pldock->clear();
        pldock->append(QStringList(dirname), 1);
        imgView->showImage(pldock->currentFilePath());
        updateWindowState();
    }
}

void MainWindow::menu_file_settings_triggered()
{
    SettingsDialog dialog(this);
    dialog.exec();
}

/******************* view *******************/
void MainWindow::menu_view_fullsize_triggered()
{
    changeCheckedScaleMenu(menu_view_fullsize, ImageViewer::FULLSIZE);
}

void MainWindow::menu_view_fitwindow_triggered()
{
    changeCheckedScaleMenu(menu_view_fitwindow, ImageViewer::FIT_WINDOW);
}

void MainWindow::menu_view_fitimage_triggered()
{
    changeCheckedScaleMenu(menu_view_fitimage, ImageViewer::FIT_IMAGE);
}

void MainWindow::menu_view_setscale_triggered()
{
    SettingScaleDialog dialog(this);
    if (dialog.getScale(imgView->getScale()))
    {
        changeCheckedScaleMenu(menu_view_setscale, ImageViewer::CUSTOM_SCALE,
                               dialog.getValue());
    }
    else
    {
        menu_view_setscale->setChecked(false);
    }
}

void MainWindow::menu_view_fullscreen_triggered()
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

/******************* slideshow *******************/
void MainWindow::menu_slideshow_slideshow_triggered()
{
    if (slideshow.isActive())
    {
        slideshow.stop();
    }
    else
    {
        slideshow.start(3000);
    }
}

/******************* window *******************/
void MainWindow::menu_window_hide_triggered()
{
    setWindowState(Qt::WindowMinimized);
}

void MainWindow::menu_window_playlist_triggered()
{
    pldock->setVisible(!pldock->isVisible());
}

void MainWindow::menu_window_histgram_triggered()
{
    histdialog->releaseHistgramImage();
    if (!imgView->empty())
    {
        histdialog->setHistgram(imgView->histgram());
        histdialog->show();
    }
}

/******************* help *******************/
void MainWindow::menu_help_version_triggered()
{
    VersionDialog *dialog = new VersionDialog(this);
    dialog->show();
}

/******************* util *******************/
void MainWindow::updateWindowState()
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
        setWindowTitle(title);
    }
}

/******************* playlist event *******************/
void MainWindow::playlistVisibleChanged(bool visible)
{
    menu_window_playlist->setChecked(visible);
}

void MainWindow::playlistItemRemoved(bool currentFile)
{
    if (pldock->empty())
    {
        imgView->releaseImage();
    }
    else if (currentFile && !pldock->empty())
    {
        imgView->showImage(pldock->currentFilePath());
    }
    updateWindowState();
}

void MainWindow::playlistItemOpened(QString path)
{
    imgView->showImage(path);
    updateWindowState();
}

/******************* image viewer event *******************/
void MainWindow::viewerRightClicked()
{
    if (pldock->empty())
    {
        return;
    }
    imgView->showImage(pldock->previousFilePath());
    updateWindowState();
}

void MainWindow::viewerLeftClicked()
{
    if (pldock->empty())
    {
        return;
    }
    imgView->showImage(pldock->nextFilePath());
    updateWindowState();
}

void MainWindow::viewerDropItems(QStringList list, bool copy)
{
    bool isempty = pldock->empty();
    if (!copy)
    {
        pldock->clear();
    }

    pldock->append(list, 1);

    if (!copy || isempty)
    {
        imgView->showImage(pldock->currentFilePath());
    }
    updateWindowState();
}

void MainWindow::viewerSetNewImage()
{
    if (histdialog->isVisible())
    {

        histdialog->releaseHistgramImage();
        if (!imgView->empty())
        {
            histdialog->setHistgram(imgView->histgram());
        }
    }
}

void MainWindow::slideshow_Timer()
{
    if (pldock->empty())
    {
        slideshow.stop();
        menu_slideshow_slideshow->setChecked(false);
    }
    else
    {
        imgView->showImage(pldock->nextFilePath());
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange)
    {
        menu_view_fullscreen->setChecked(isFullScreen());
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    saveSettings();
}

/******************* util *******************/
void MainWindow::createMenus()
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
    menu_view_fullscreen = new QAction(tr("フルスクリーン"), this);
    menu_view_fullscreen->setCheckable(true);
    menu_view_fullscreen->setShortcut(tr("Meta+Ctrl+F"));
    menu_view_filter = new QAction(tr("フィルター"), this);
    menu_view->addAction(menu_view_fullsize);
    menu_view->addAction(menu_view_fitwindow);
    menu_view->addAction(menu_view_fitimage);
    menu_view->addAction(menu_view_setscale);
    menu_view->addSeparator();
    menu_view->addAction(menu_view_fullscreen);
    menu_view->addSeparator();
    menu_view->addAction(menu_view_filter);
    connect(menu_view_fullsize, SIGNAL(triggered()), this, SLOT(menu_view_fullsize_triggered()));
    connect(menu_view_fitwindow, SIGNAL(triggered()), this, SLOT(menu_view_fitwindow_triggered()));
    connect(menu_view_fitimage, SIGNAL(triggered()), this, SLOT(menu_view_fitimage_triggered()));
    connect(menu_view_setscale, SIGNAL(triggered()), this, SLOT(menu_view_setscale_triggered()));
    connect(menu_view_fullscreen, SIGNAL(triggered()), this, SLOT(menu_view_fullscreen_triggered()));
    menuBar()->addMenu(menu_view);

    menu_slideshow = new QMenu(this);
    menu_slideshow->setTitle(tr("スライドショー"));
    menu_slideshow_slideshow = new QAction(tr("スライドショー"), this);
    menu_slideshow_slideshow->setCheckable(true);
    menu_slideshow->addAction(menu_slideshow_slideshow);
    connect(menu_slideshow_slideshow, SIGNAL(triggered()), this, SLOT(menu_slideshow_slideshow_triggered()));
    menuBar()->addMenu(menu_slideshow);

    menu_window = new QMenu(this);
    menu_window->setTitle(tr("ウィンドウ"));
    menu_window_hide = new QAction(tr("最小化"), this);
    menu_window_hide->setShortcut(tr("Ctrl+M"));
    menu_window_playlist = new QAction(tr("プレイリスト"), this);
    menu_window_playlist->setCheckable(true);
    menu_window_histgram = new QAction(tr("ヒストグラム"), this);
    menu_window_torncurve = new QAction(tr("トーンカーブ"), this);
    menu_window->addAction(menu_window_hide);
    menu_window->addAction(menu_window_playlist);
    menu_window->addAction(menu_window_histgram);
    menu_window->addAction(menu_window_torncurve);
    connect(menu_window_hide, SIGNAL(triggered()), this, SLOT(menu_window_hide_triggered()));
    connect(menu_window_playlist, SIGNAL(triggered()), this, SLOT(menu_window_playlist_triggered()));
    connect(menu_window_histgram, SIGNAL(triggered()), this, SLOT(menu_window_histgram_triggered()));
    menuBar()->addMenu(menu_window);

    menu_help = new QMenu(this);
    menu_help->setTitle(tr("ヘルプ"));
    menu_help_version = new QAction(tr("バージョン"), this);
    menu_help->addAction(menu_help_version);
    connect(menu_help_version, SIGNAL(triggered()), this, SLOT(menu_help_version_triggered()));
    menuBar()->addMenu(menu_help);
}

void MainWindow::changeCheckedScaleMenu(QAction *act, ImageViewer::ViewMode m, qreal s)
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
    updateWindowState();
}

void MainWindow::saveSettings()
{
    QSettings settings(BookReader::SOFTWARE_ORGANIZATION, BookReader::SOFTWARE_NAME, this);

    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("location", pos());
    settings.endGroup();

    settings.beginGroup("Main");
    settings.setValue("dialog_file", dialog_File);
    settings.setValue("dialog_directory", dialog_Directory);
    settings.endGroup();

    settings.beginGroup("Viewer");
    settings.setValue("scaling_mode", imgView->getScaleMode());
    settings.setValue("scaling_times", imgView->getScale());
    settings.endGroup();

    settings.beginGroup("Playlist");
    settings.setValue("visible", pldock->isVisible());
    settings.endGroup();
}

void MainWindow::restoreSettings()
{
    QSettings settings(BookReader::SOFTWARE_ORGANIZATION, BookReader::SOFTWARE_NAME, this);

    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(600, 400)).toSize());
    move(settings.value("location", QPoint(100, 100)).toPoint());
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
                               settings.value("scaling_times", 0.0).toFloat());
        break;
    }
    settings.endGroup();

    settings.beginGroup("Playlist");
    pldock->setVisible(settings.value("visible", false).toBool());
    menu_window_playlist->setChecked(pldock->isVisible());
    settings.endGroup();
}
