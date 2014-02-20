#include "imagemanager.h"

ImageManager::ImageManager(QGraphicsView *view)
{
    this->view = view;

    showingIndex = -1;

    // ポインタの初期化
    view_img = nullptr;
    view_gscene = nullptr;
    view_gitem = nullptr;
}

ImageManager::~ImageManager()
{
    delete view_img;
    delete view_gitem;
    delete view_gscene;
}

bool ImageManager::loadFile(const QString &path)
{
    if (path.isEmpty())
    {
        return false;
    }

    imgList.clear();
    imgList << path;

    return showImage(0);
}

bool ImageManager::loadDir(const QString &path)
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

void ImageManager::nextImage()
{
    if (showingIndex >= 0)
    {
        showingIndex++;
        if (showingIndex >= imgList.length())
        {
            showingIndex = 0;
        }
        showImage(showingIndex);
    }
}

void ImageManager::previousImage()
{
    if (showingIndex >= 0)
    {
        showingIndex--;
        if (showingIndex < 0)
        {
            showingIndex = imgList.length()-1;
        }
        showImage(showingIndex);
    }
}

void ImageManager::releaseImages()
{
    delete view_img;
    delete view_gitem;
    delete view_gscene;

    view_img = nullptr;
    view_gitem = nullptr;
    view_gscene = nullptr;

    imgList.clear();
    showingIndex = -1;
}

QStringList ImageManager::getImageList()
{

}

int ImageManager::getImageListCount()
{
    return imgList.count();
}

QStringList ImageManager::getReadableExtension()
{
    QStringList list;
    for (int i = 0; i < extListLen; i++)
    {
        list << extList[i];
    }
    return list;
}

bool ImageManager::isReadable(const QString &path)
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

int ImageManager::getImageListIndex()
{
    return showingIndex;
}

QString ImageManager::getShowingFileName()
{
    if (showingIndex < 0 || showingIndex >= imgList.size())
    {
        return QString();
    }

    QFileInfo info(imgList.at(showingIndex));
    return info.fileName();
}

QSize ImageManager::getShowingImageSize()
{
    if (showingIndex >= 0)
    {
        return view_img->size();
    }
    return QSize(0, 0);
}

bool ImageManager::showImage(int n)
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

    QGraphicsScene *scene = new QGraphicsScene();
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(*img));
    view->setScene(scene);
    scene->addItem(item);

    delete view_img;
    delete view_gitem;
    delete view_gscene;

    view_gscene = scene;
    view_img = img;
    view_gitem = item;
    showingIndex = n;

    return true;
}

QString ImageManager::connectFilePath(QString parent, QString child)
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
