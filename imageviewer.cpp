#include "imageviewer.h"

const QString ImageViewer::extList[5] = {"jpg", "png", "jpeg", "bmp", "gif"};
const int ImageViewer::extListLen = 5;

ImageViewer::ImageViewer() : QGraphicsView(),
    view_img(nullptr), view_scene(new QGraphicsScene()), view_item(nullptr),
    showingIndex(-1), img_scale(1.0), mode(FULLSIZE)
{
    setScene(view_scene);
}

ImageViewer::~ImageViewer()
{
    delete view_img;
    delete view_item;
    delete view_scene;
}

/***************** 画像読み込み *******************/
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
    openDir(path);

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

    imageChanged();
}

/***************** getter *******************/
QStringList ImageViewer::getReadableExtension() const
{
    QStringList list;
    for (int i = 0; i < extListLen; i++)
    {
        list << extList[i];
    }
    return list;
}

int ImageViewer::getImageListCount() const
{
    return imgList.count();
}

int ImageViewer::getImageListIndex() const
{
    return showingIndex;
}

QString ImageViewer::getFileName() const
{
    if (showingIndex < 0 || showingIndex >= imgList.size())
    {
        return QString();
    }

    QFileInfo info(imgList.at(showingIndex));
    return info.fileName();
}

QString ImageViewer::getFilePath() const
{
    if (showingIndex < 0 || showingIndex >= imgList.size())
    {
        return QString();
    }

    return imgList.at(showingIndex);
}

QSize ImageViewer::getImageSize() const
{
    if (showingIndex >= 0)
    {
        return view_img->size();
    }
    return QSize(0, 0);
}

qreal ImageViewer::getScale() const
{
    return img_scale;
}

ImageViewer::ViewMode ImageViewer::getScaleMode() const
{
    return mode;
}

/***************** setter *******************/
ImageViewer& ImageViewer::setAntiAliasing(bool b)
{
    setRenderHint(QPainter::Antialiasing, b);
    return *this;
}

ImageViewer& ImageViewer::setScale(ViewMode m, qreal s)
{
    mode = m;
    img_scale = s;
    setupMatrix();
    return *this;
}

ImageViewer& ImageViewer::setScale(ViewMode m)
{
    mode = m;
    setupMatrix();
    return *this;
}

bool ImageViewer::isReadable(const QString &path) const
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

/***************** private *******************/
/***************** event *******************/
void ImageViewer::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    QGraphicsView::resizeEvent(event);
    setupMatrix();
    sizeChanged();
}

void ImageViewer::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void ImageViewer::dragLeaveEvent(QDragLeaveEvent *event)
{
    // 何もしてないオーバーライドしているメソッドですが、
    // 消すとD&Dがうまくいきません
    Q_UNUSED(event);
}

void ImageViewer::dragMoveEvent(QDragMoveEvent *event)
{
    // 何もしてないオーバーライドしているメソッドですが、
    // 消すとD&Dがうまくいきません
    Q_UNUSED(event);
}

void ImageViewer::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();
    QList<QUrl> urls = mime->urls();

    if ((event->keyboardModifiers() & Qt::ControlModifier) != Qt::ControlModifier)
    {
        imgList.clear();
    }
    QList<QUrl>::const_iterator iterator;
    for (iterator = urls.constBegin(); iterator != urls.constEnd(); ++iterator)
    {
        QString path = (*iterator).toLocalFile();
        QFileInfo info(path);
        if (info.exists())
        {
            if (info.isDir())
            {
                openDir(path);
            }
            else if (isReadable(path))
            {
                imgList << path;
            }
        }
    }
    showImage(0);
}

void ImageViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        nextImage();
    }
    if (event->buttons() & Qt::RightButton)
    {
        previousImage();
    }
}

/***************** util *******************/
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
    imageChanged();
    QScrollBar *vScrollBar = verticalScrollBar();
    vScrollBar->setSliderPosition(0);

    return true;
}

void ImageViewer::openDir(const QString &path)
{
    QDir dir(path);
    if (dir.exists())
    {
        QStringList list = dir.entryList();
        QStringList::const_iterator iterator;
        for (iterator = list.constBegin(); iterator != list.constEnd(); ++iterator)
        {
            if (isReadable(*iterator))
            {
                imgList << connectFilePath(path, *iterator);
            }
        }
    }
}

void ImageViewer::setupMatrix()
{
    QMatrix matrix;

    if (mode == ImageViewer::CUSTOM_SCALE)
    {
        matrix.scale(img_scale, img_scale);
    }
    else if (mode == ImageViewer::FULLSIZE)
    {
        img_scale = 1.0;
    }
    else
    {
        qreal ws = 1.0, hs = 1.0, s = 1.0;
        if (size().width() < getImageSize().width())
        {
            ws = (qreal)size().width() / (qreal)getImageSize().width();
        }
        if (size().height() < getImageSize().height())
        {
            hs = (qreal)size().height() / (qreal)getImageSize().height();
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
        img_scale = s;
    }

    setMatrix(matrix);
}

QString ImageViewer::connectFilePath(const QString &parent, const QString &child) const
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
