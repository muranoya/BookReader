#include "imagemanager.h"

ImageManager::ImageManager(QGraphicsView *view)
{
    this->view = view;

    this->showingIndex = -1;

    // ポインタの初期化
    this->view_img = nullptr;
    this->view_gscene = nullptr;
    this->view_gitem = nullptr;
}

ImageManager::~ImageManager()
{
    delete this->view_img;
    delete this->view_gitem;
    delete this->view_gscene;
}

bool ImageManager::loadFile(const QString &path)
{
    if (path.isEmpty())
    {
        return false;
    }

    this->imgList.clear();
    this->imgList << path;

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

    this->imgList.clear();
    QStringList list = dir.entryList();
    QStringList::const_iterator iterator;
    for (iterator = list.constBegin(); iterator != list.constEnd(); ++iterator)
    {
        if (isReadable(*iterator))
        {
            this->imgList << connectFilePath(path, *iterator);
        }
    }

    return showImage(0);
}

void ImageManager::nextImage()
{
    if (this->showingIndex >= 0)
    {
        this->showingIndex++;
        if (this->showingIndex >= this->imgList.length())
        {
            this->showingIndex = 0;
        }
        showImage(this->showingIndex);
    }
}

void ImageManager::previousImage()
{
    if (this->showingIndex >= 0)
    {
        this->showingIndex--;
        if (this->showingIndex < 0)
        {
            this->showingIndex = this->imgList.length()-1;
        }
        showImage(this->showingIndex);
    }
}

void ImageManager::releaseImages()
{
    delete this->view_img;
    delete this->view_gitem;
    delete this->view_gscene;

    this->view_img = nullptr;
    this->view_gitem = nullptr;
    this->view_gscene = nullptr;

    this->imgList.clear();
    this->showingIndex = -1;
}

QStringList ImageManager::getImageList()
{

}

int ImageManager::getImageListCount()
{
    return this->imgList.count();
}

QStringList ImageManager::getReadableExtension()
{
    QStringList list;
    for (int i = 0; i < this->extListLen; i++)
    {
        list << this->extList[i];
    }
    return list;
}

bool ImageManager::isReadable(const QString &path)
{
    QFileInfo info(path);
    QString ext = info.suffix().toLower();
    for (int i = 0; i < this->extListLen; i++)
    {
        if (ext == this->extList[i])
        {
            return true;
        }
    }
    return false;
}

int ImageManager::getImageListIndex()
{
    return this->showingIndex;
}

QString ImageManager::getShowingFileName()
{
    if (this->showingIndex < 0 || this->showingIndex >= this->imgList.size())
    {
        return QString();
    }

    QFileInfo info(this->imgList.at(this->showingIndex));
    return info.fileName();
}

QSize ImageManager::getShowingImageSize()
{
    if (this->showingIndex >= 0)
    {
        return this->view_img->size();
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

    delete this->view_img;
    delete this->view_gitem;
    delete this->view_gscene;

    this->view_gscene = scene;
    this->view_img = img;
    this->view_gitem = item;
    this->showingIndex = n;

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
