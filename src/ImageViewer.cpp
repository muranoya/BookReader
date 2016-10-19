#include "ImageViewer.hpp"

ImageViewer::ImageViewer(QWidget *parent)
    : Viewer(parent)
    , plmodel()
{
    connect(this, SIGNAL(openImageFiles(const QStringList &)),
            &plmodel, SLOT(openImages(const QStringList &)));
    connect(this, SIGNAL(nextImageRequest()),
            &plmodel, SLOT(nextImage()));
    connect(this, SIGNAL(prevImageRequest()),
            &plmodel, SLOT(prevImage()));
    connect(this, SIGNAL(changeNumOfImages(int)),
            &plmodel, SLOT(changeNumOfImages(int)));

    connect(&plmodel, SIGNAL(changeImages(const QImage &, const QImage &)),
            this, SLOT(showImages(const QImage &, const QImage &)));
    connect(&plmodel, SIGNAL(changePlaylistStatus()),
            this, SLOT(changedStatus()));
}

ImageViewer::~ImageViewer()
{
}

void
ImageViewer::openImages(const QStringList &paths)
{
    plmodel.openImages(paths);
}

void
ImageViewer::setOpenDirLevel(int n)
{
    plmodel.setOpenDirLevel(n);
}

int
ImageViewer::getOpenDirLevel() const
{
    return plmodel.getOpenDirLevel();
}

void
ImageViewer::setCacheSize(int n)
{
    plmodel.setCacheSize(n);
}

int
ImageViewer::getCacheSize() const
{
    return plmodel.getCacheSize();
}

int
ImageViewer::countShowImages() const
{
    return plmodel.countShowImages();
}

int
ImageViewer::count() const
{
    return plmodel.count();
}

bool
ImageViewer::empty() const
{
    return plmodel.empty();
}

int
ImageViewer::currentIndex(int i) const
{
    return plmodel.currentIndex(i);
}

QString
ImageViewer::currentFileName(int i) const
{
    return plmodel.currentFileName(i);
}

void
ImageViewer::setModelToItemView(QAbstractItemView *view)
{
    plmodel.setModelToItemView(view);
}

void
ImageViewer::showSelectedItem()
{
    plmodel.showSelectedItem();
}

void
ImageViewer::removeSelectedItem()
{
    plmodel.removeSelectedItem();
}

void
ImageViewer::clearPlaylist()
{
    plmodel.clearPlaylist();
}

void
ImageViewer::changedStatus()
{
    emit changeImageViewerStatus();
}

