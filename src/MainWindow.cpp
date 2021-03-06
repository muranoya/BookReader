#include <QDir>
#include <QCoreApplication>
#include "MainWindow.hpp"
#include "ScaleDialog.hpp"
#include "App.hpp"
#include "SettingDialog.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , viewer(new ImageViewer())
    , plview(new QListView())
    , dockwidget(new QDockWidget(tr("Playlist"), this))
    , lastdir()
{
    addDockWidget(Qt::LeftDockWidgetArea, dockwidget);
    setCentralWidget(viewer);
    dockwidget->setWidget(plview);
    viewer->setModelToItemView(plview);
    plview->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(viewer, SIGNAL(changeImageViewerStatus()),
            this, SLOT(updateWindowText()));

    createMenus();

    App::LoadSettings();
    applySettings();
    updateWindowText();

    QStringList args = QCoreApplication::arguments();
    args.removeFirst();
    viewer->openImages(args);
}

MainWindow::~MainWindow()
{
}

/******************* file *******************/
void
MainWindow::menu_file_open_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(
                this, tr("Select Files"), lastdir,
                ImageFile::readableFormatExt());

    if (!files.isEmpty())
    {
        const QDir dir = QFileInfo(files.at(0)).absoluteDir();
        lastdir = dir.absolutePath();
        viewer->clearPlaylist();
        viewer->openImages(files);
    }
}

void
MainWindow::menu_file_fopen_triggered()
{
    QString dirname = QFileDialog::getExistingDirectory(
                this, tr("Select Directory"), lastdir);

    if (!dirname.isEmpty())
    {
        lastdir = dirname;
        viewer->clearPlaylist();
        viewer->openImages(QStringList(dirname));
    }
}

void
MainWindow::menu_file_settings_triggered()
{
    if (SettingDialog::openSettingDialog())
    {
        viewer->setOpenDirLevel(App::view_openlevel);
        viewer->setCacheSize(App::pl_prefetch);
        viewer->setFeedPageMode(
                static_cast<Viewer::FeedPageMode>(App::view_feedpage));
    }
}

/******************* view *******************/
void
MainWindow::menu_view_fullsize_triggered()
{
    changeCheckedScaleMenu(menu_view_fullsize, Viewer::ActualSize);
}

void
MainWindow::menu_view_fitwindow_triggered()
{
    changeCheckedScaleMenu(menu_view_fitwindow, Viewer::FittingWindow);
}

void
MainWindow::menu_view_fitwidth_triggered()
{
    changeCheckedScaleMenu(menu_view_fitwidth, Viewer::FittingWidth);
}

void
MainWindow::menu_view_setscale_triggered()
{
    double ret;
    if (ScaleDialog::getScale(viewer->getImageSize(),
                viewer->getCustomScaleFactor(), ret))
    {
        changeCheckedScaleMenu(menu_view_setscale,
                Viewer::CustomScale, ret);
    }
    else
    {
        menu_view_setscale->setChecked(false);
    }
}

void
MainWindow::menu_view_spread_triggered()
{
    viewer->setSpreadView(menu_view_spread->isChecked());
}

void
MainWindow::menu_view_autospread_triggered()
{
    viewer->setAutoAdjustSpread(menu_view_autospread->isChecked());
}

void
MainWindow::menu_view_rightbinding_triggered()
{
    viewer->setRightbindingView(menu_view_rightbinding->isChecked());
}

void
MainWindow::menu_view_nn_triggered()
{
    if (menu_view_nn->isChecked())
    {
        menu_view_bi->setChecked(false);
        menu_view_bc->setChecked(false);
    }
    else
    {
        menu_view_nn->setChecked(true);
        menu_view_bi->setChecked(false);
        menu_view_bc->setChecked(false);
    }
    viewer->setScalingMode(Viewer::NearestNeighbor);
}

void
MainWindow::menu_view_bi_triggered()
{
    if (menu_view_bi->isChecked())
    {
        menu_view_nn->setChecked(false);
        menu_view_bc->setChecked(false);
    }
    else
    {
        menu_view_nn->setChecked(false);
        menu_view_bi->setChecked(true);
        menu_view_bc->setChecked(false);
    }
    viewer->setScalingMode(Viewer::Bilinear);
}

void
MainWindow::menu_view_bc_triggered()
{
    if (menu_view_bc->isChecked())
    {
        menu_view_nn->setChecked(false);
        menu_view_bi->setChecked(false);
    }
    else
    {
        menu_view_nn->setChecked(false);
        menu_view_bi->setChecked(false);
        menu_view_bc->setChecked(true);
    }
    viewer->setScalingMode(Viewer::Bicubic);
}

/******************* util *******************/
void
MainWindow::updateWindowText()
{
    QString title;
    if (viewer->empty())
    {
        title = tr("%1").arg(App::SOFTWARE_NAME);
    }
    else
    {
        if (viewer->countShowImages() == 1)
        {
            title = tr("[%1/%2]")
                .arg(viewer->currentIndex(0)+1)
                .arg(viewer->count());
        }
        else
        {
            title = tr("[%1-%2/%3]")
                .arg(viewer->currentIndex(0)+1)
                .arg(viewer->currentIndex(1)+1)
                .arg(viewer->count());
        }

        if (viewer->countShowImages() == 2)
        {
            title = (viewer->getRightbindingView() ? tr("%1 %3 | %2")
                                                    : tr("%1 %2 | %3"))
                .arg(title)
                .arg(viewer->currentFileName(0))
                .arg(viewer->currentFileName(1));
        }
        else
        {
            title = tr("%1 %2")
                .arg(title)
                .arg(viewer->currentFileName(0));
        }

        title = tr("%1 %3%")
            .arg(title)
            .arg(QString::number(viewer->getCustomScaleFactor() * 100.0, 'g', 4));
    }
    setWindowTitle(title);
}

void
MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    storeSettings();
    App::SaveSettings();
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
    pl_show =   new QAction(tr("Show"), plview);
    pl_sep1 =   new QAction(plview);
    pl_sep1->setSeparator(true);
    pl_remove = new QAction(tr("Remove"), plview);
    pl_clear =  new QAction(tr("Clear"), plview);
    
    plview->setContextMenuPolicy(Qt::ActionsContextMenu);
    plview->addAction(pl_show);
    plview->addAction(pl_sep1);
    plview->addAction(pl_remove);
    plview->addAction(pl_clear);
    connect(pl_show, SIGNAL(triggered()),
            viewer, SLOT(showSelectedItem()));
    connect(pl_remove, SIGNAL(triggered()),
            viewer, SLOT(removeSelectedItem()));
    connect(pl_clear, SIGNAL(triggered()),
            viewer, SLOT(clearPlaylist()));

    menu_file = new QMenu(tr("File"), this);
    menu_file_open     = new QAction(tr("Open"), this);
    menu_file_fopen    = new QAction(tr("Open Directory"), this);
    menu_file_settings = new QAction(tr("Configuration"), this);
    menu_file_aboutqt  = new QAction(tr("About Qt"), this);
    menu_file_exit     = new QAction(tr("Exit"), this);

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

    menu_view = new QMenu(tr("Display"), this);
    menu_view_fullsize     = new QAction(tr("Actual Size"), this);
    menu_view_fullsize->setCheckable(true);
    menu_view_fitwindow    = new QAction(tr("Fit Window"), this);
    menu_view_fitwindow->setCheckable(true);
    menu_view_fitwidth     = new QAction(tr("Fit Width"), this);
    menu_view_fitwidth->setCheckable(true);
    menu_view_setscale     = new QAction(tr("Specify Magnification"), this);
    menu_view_setscale->setCheckable(true);
    menu_view_spread       = new QAction(tr("Spread"), this);
    menu_view_spread->setCheckable(true);
    menu_view_autospread   = new QAction(tr("Auto Spread"), this);
    menu_view_autospread->setCheckable(true);
    menu_view_rightbinding = new QAction(tr("Right Binding"), this);
    menu_view_rightbinding->setCheckable(true);
    menu_view_nn           = new QAction(tr("Low (Nearest Neighbor)"), this);
    menu_view_nn->setCheckable(true);
    menu_view_bi           = new QAction(tr("Balance (Bilinear)"), this);
    menu_view_bi->setCheckable(true);
    menu_view_bc           = new QAction(tr("High (Bicubic)"), this);
    menu_view_bc->setCheckable(true);

    menu_view->addAction(menu_view_fullsize);
    menu_view->addAction(menu_view_fitwindow);
    menu_view->addAction(menu_view_fitwidth);
    menu_view->addAction(menu_view_setscale);
    menu_view->addSeparator();
    menu_view->addAction(menu_view_spread);
    menu_view->addAction(menu_view_autospread);
    menu_view->addAction(menu_view_rightbinding);
    menu_view->addSeparator();
    menu_view->addAction(menu_view_nn);
    menu_view->addAction(menu_view_bi);
    menu_view->addAction(menu_view_bc);
    connect(menu_view_fullsize,     SIGNAL(triggered()),
            this, SLOT(menu_view_fullsize_triggered()));
    connect(menu_view_fitwindow,    SIGNAL(triggered()),
            this, SLOT(menu_view_fitwindow_triggered()));
    connect(menu_view_fitwidth,     SIGNAL(triggered()),
            this, SLOT(menu_view_fitwidth_triggered()));
    connect(menu_view_setscale,     SIGNAL(triggered()),
            this, SLOT(menu_view_setscale_triggered()));
    connect(menu_view_spread,       SIGNAL(triggered()),
            this, SLOT(menu_view_spread_triggered()));
    connect(menu_view_autospread,   SIGNAL(triggered()),
            this, SLOT(menu_view_autospread_triggered()));
    connect(menu_view_rightbinding, SIGNAL(triggered()),
            this, SLOT(menu_view_rightbinding_triggered()));
    connect(menu_view_nn,           SIGNAL(triggered()),
            this, SLOT(menu_view_nn_triggered()));
    connect(menu_view_bi,           SIGNAL(triggered()),
            this, SLOT(menu_view_bi_triggered()));
    connect(menu_view_bc,           SIGNAL(triggered()),
            this, SLOT(menu_view_bc_triggered()));
    menuBar()->addMenu(menu_view);


    menu_window = new QMenu(tr("Window"), this);
    menu_window->addAction(dockwidget->toggleViewAction());

    menuBar()->addMenu(menu_window);
}

void
MainWindow::changeCheckedScaleMenu(QAction *act,
        Viewer::ViewMode m, double s)
{
    menu_view_fullsize->setChecked(false);
    menu_view_fitwindow->setChecked(false);
    menu_view_fitwidth->setChecked(false);
    menu_view_setscale->setChecked(false);

    act->setChecked(true);

    if (m == Viewer::CustomScale)
    {
        viewer->setViewMode(m, s);
    }
    else
    {
        viewer->setViewMode(m);
    }
}

void
MainWindow::applySettings()
{
    resize(App::mw_size);
    move(App::mw_pos);

    Viewer::ViewMode vmode = 
        static_cast<Viewer::ViewMode>(App::view_scalem);
    switch (vmode)
    {
        case Viewer::ActualSize:
            changeCheckedScaleMenu(menu_view_fullsize, vmode);
            break;
        case Viewer::FittingWindow:
            changeCheckedScaleMenu(menu_view_fitwindow, vmode);
            break;
        case Viewer::FittingWidth:
            changeCheckedScaleMenu(menu_view_fitwidth, vmode);
            break;
        case Viewer::CustomScale:
            changeCheckedScaleMenu(menu_view_setscale, vmode,
                    App::view_scale);
            break;
    }
    
    viewer->setScalingMode(
            static_cast<Viewer::ScalingMode>(App::view_ipix));
    switch (viewer->getScalingMode())
    {
        case Viewer::NearestNeighbor:
            menu_view_nn->setChecked(true);
            break;
        case Viewer::Bilinear:
            menu_view_bi->setChecked(true);
            break;
        case Viewer::Bicubic:
            menu_view_bc->setChecked(true);
            break;
    }

    viewer->setSpreadView(App::view_spread);
    menu_view_spread->setChecked(App::view_spread);

    viewer->setAutoAdjustSpread(App::view_autospread);
    menu_view_autospread->setChecked(App::view_autospread);

    viewer->setRightbindingView(App::view_rbind);
    menu_view_rightbinding->setChecked(App::view_rbind);
    
    viewer->setFeedPageMode(
            static_cast<Viewer::FeedPageMode>(App::view_feedpage));

    viewer->setOpenDirLevel(App::view_openlevel);

    dockwidget->setVisible(App::pl_visible);

    viewer->setCacheSize(App::pl_prefetch);
}

void
MainWindow::storeSettings()
{
    App::mw_size = size();
    App::mw_pos = pos();

    App::view_scalem     = static_cast<int>(viewer->getViewMode());
    App::view_scale      = viewer->getCustomScaleFactor();
    App::view_ipix       = static_cast<int>(viewer->getScalingMode());
    App::view_spread     = menu_view_spread->isChecked();
    App::view_autospread = menu_view_autospread->isChecked();
    App::view_rbind      = menu_view_rightbinding->isChecked();
    App::view_openlevel  = viewer->getOpenDirLevel();
    App::view_feedpage   =
        static_cast<Viewer::FeedPageMode>(viewer->getFeedPageMode());

    App::pl_visible  = dockwidget->isVisible();
    App::pl_prefetch = viewer->getCacheSize();
}

