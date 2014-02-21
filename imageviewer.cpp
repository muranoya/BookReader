#include "imageviewer.h"

ImageViewer::ImageViewer()
{
    showingIndex = -1;

    img_rotate = 0.0;
    img_scale = 1.0;
    mode = FULLSIZE;

    view_scene = new QGraphicsScene();
    setScene(view_scene);

    // ポインタの初期化
    view_img = nullptr;
    //view_scene = nullptr;
    view_item = nullptr;
}

ImageViewer::~ImageViewer()
{
    delete view_img;
    delete view_item;
    delete view_scene;
}

bool ImageViewer::loadFile(const QString &path)
{
    if (path.isEmpty())
    {
        return false;
    }

    imgList.clear();
    imgList << path;

    return showImage(0);
}

bool ImageViewer::loadDir(const QString &path)
{
    if (path.isEmpty())
    {
        return false;
    }

    QDir dir(path);
    if (!dir.exists())
    {
        return false;
    }

    imgList.clear();
    QStringList list = dir.entryList();
    QStringList::const_iterator iterator;
    for (iterator = list.constBegin(); iterator != list.constEnd(); ++iterator)
    {
        if (isReadable(*iterator))
        {
            imgList << connectFilePath(path, *iterator);
        }
    }

    return showImage(0);
}

void ImageViewer::nextImage()
{
    if (showingIndex >= 0)
    {
        int oldindex = showingIndex;
        showingIndex++;
        if (showingIndex >= imgList.length())
        {
            showingIndex = 0;
        }
        if (showingIndex != oldindex)
        {
            showImage(showingIndex);
        }
    }
}

void ImageViewer::previousImage()
{
    if (showingIndex >= 0)
    {
        int oldindex = showingIndex;
        showingIndex--;
        if (showingIndex < 0)
        {
            showingIndex = imgList.length()-1;
        }
        if (showingIndex != oldindex)
        {
            showImage(showingIndex);
        }
    }
}

void ImageViewer::releaseImages()
{
    delete view_img;
    delete view_item;

    view_img = nullptr;
    view_item = nullptr;

    imgList.clear();
    showingIndex = -1;
}

QStringList ImageViewer::getImageList()
{

}

int ImageViewer::getImageListCount()
{
    return imgList.count();
}

QStringList ImageViewer::getReadableExtension()
{
    QStringList list;
    for (int i = 0; i < extListLen; i++)
    {
        list << extList[i];
    }
    return list;
}

bool ImageViewer::isReadable(const QString &path)
{
    QFileInfo info(path);
    QString ext = info.suffix().toLower();
    for (int i = 0; i < extListLen; i++)
    {
        if (ext == extList[i])
        {
            return true;
        }
    }
    return false;
}

int ImageViewer::getImageListIndex()
{
    return showingIndex;
}

QString ImageViewer::getShowingFileName()
{
    if (showingIndex < 0 || showingIndex >= imgList.size())
    {
        return QString();
    }

    QFileInfo info(imgList.at(showingIndex));
    return info.fileName();
}

void ImageViewer::setAntiAliasing(bool b)
{
    setRenderHint(QPainter::Antialiasing, b);
}

void ImageViewer::setScrollHand(bool b)
{
    setDragMode(b ? QGraphicsView::ScrollHandDrag
                  : QGraphicsView::NoDrag);
    setInteractive(!b);
}

void ImageViewer::setScale(ViewMode m)
{
    mode = m;
    setupMatrix();
}

void ImageViewer::setScale(ViewMode m, qreal s)
{
    mode = m;
    img_scale = s;
    setupMatrix();
}

void ImageViewer::setRotate(qreal deg)
{
    img_rotate = deg;
    setupMatrix();
}

qreal ImageViewer::getScale()
{
    return img_scale;
}

qreal ImageViewer::getRotate()
{
    return img_rotate;
}

QSize ImageViewer::getShowingImageSize()
{
    if (showingIndex >= 0)
    {
        return view_img->size();
    }
    return QSize(0, 0);
}

bool ImageViewer::showImage(int n)
{
    if (imgList.size() <= n || n < 0)
    {
        return false;
    }

    QImage *img = new QImage(imgList.at(n));

    if (img->isNull())
    {
        delete img;
        return false;
    }

    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(*img));
    view_scene->addItem(item);
    view_scene->setSceneRect(0.0, 0.0, img->width(), img->height());

    delete view_img;
    delete view_item;

    view_img = img;
    view_item = item;
    showingIndex = n;

    setupMatrix();

    return true;
}

void ImageViewer::setupMatrix()
{
    QMatrix matrix;

    if (mode != ImageViewer::FULLSIZE)
    {
        qreal ws = 1.0, hs = 1.0, s = 1.0;
        if (size().width() < getShowingImageSize().width())
        {
            ws = (qreal)size().width() / (qreal)getShowingImageSize().width();
        }
        if (size().height() < getShowingImageSize().height())
        {
            hs = (qreal)size().height() / (qreal)getShowingImageSize().height();
        }

        if (mode == ImageViewer::FIT_WINDOW)
        {
            s = ws > hs ? hs : ws;
        }
        else if (mode == ImageViewer::FIT_IMAGE)
        {
            s = ws;
        }

        matrix.scale(s, s);
    }

    matrix.rotate(img_rotate);

    setMatrix(matrix);
}

QString ImageViewer::connectFilePath(QString parent, QString child)
{
    if (parent.isEmpty())
    {
        return QString();
    }

    if (child.isEmpty())
    {
        return parent;
    }

    if (parent.at(parent.length()-1) == QDir::separator())
    {
        return parent + child;
    }
    else
    {
        return parent + QDir::separator() + child;
    }
}
