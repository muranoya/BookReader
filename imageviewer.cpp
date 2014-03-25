#include "imageviewer.h"

const QString ImageViewer::extList[5] = {"jpg", "png", "jpeg", "bmp", "gif"};
const int ImageViewer::extListLen = 5;

ImageViewer::ImageViewer(QMainWindow *parent) : QGraphicsView(),
    view_img(nullptr), view_scene(new QGraphicsScene()), view_item(nullptr),
    pldock(parent), img_scale(1.0), mode(FULLSIZE), slideshow(nullptr)
{
    setScene(view_scene);

    parent->addDockWidget(Qt::LeftDockWidgetArea, &pldock);
    connect(&pldock, SIGNAL(playlistItemOpen()), this, SLOT(on_PlaylistSelected()));
}

ImageViewer::~ImageViewer()
{
    delete view_img;
    delete view_item;
    delete view_scene;
    delete slideshow;
}

/***************** 画像読み込み *******************/
void ImageViewer::loadFiles(const QStringList &paths)
{
    if (paths.isEmpty())
    {
        return;
    }

    pldock.clear();

    QStringList::const_iterator iterator;
    for (iterator = paths.constBegin(); iterator != paths.constEnd(); ++iterator)
    {
        QFileInfo info(*iterator);
        if (info.exists() && isReadable(*iterator))
        {
            pldock.append(*iterator);
        }
    }

    showImage(pldock.currentIndex());
}

void ImageViewer::loadDir(const QString &path)
{
    if (path.isEmpty())
    {
        return;
    }

    QDir dir(path);
    if (!dir.exists())
    {
        return;
    }

    pldock.clear();
    openDir(path);

    showImage(pldock.currentIndex());
}

void ImageViewer::nextImage()
{
    showImage(pldock.nextIndex());
}

void ImageViewer::previousImage()
{
    showImage(pldock.previousIndex());
}

void ImageViewer::releaseImages()
{
    delete view_img;
    delete view_item;

    view_img = nullptr;
    view_item = nullptr;

    pldock.clear();

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
    return pldock.count();
}

int ImageViewer::getImageListIndex() const
{
    return pldock.currentIndex();
}

QString ImageViewer::getFileName() const
{
    return pldock.getFileName();
}

QString ImageViewer::getFilePath() const
{
    return pldock.getFilePath();
}

QSize ImageViewer::getImageSize() const
{
    return (view_img == nullptr) ? QSize(0, 0) : view_img->size();
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

/***************** other *******************/
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

void ImageViewer::startSlideShow()
{
    if (!playSlideShow())
    {
        slideshow = new QTimer(this);
        connect(slideshow, SIGNAL(timeout()), this, SLOT(update_Timer_SlideShow()));
        slideshow->start(3000);
    }
}

void ImageViewer::stopSlideShow()
{
    if (playSlideShow())
    {
        slideshow->stop();
        delete slideshow;
        slideshow = nullptr;
    }
}

bool ImageViewer::playSlideShow() const
{
    return slideshow != nullptr;
}

bool ImageViewer::isPlaylistVisibled() const
{
    return pldock.isVisible();
}

void ImageViewer::setPlaylistVisibled(bool visible)
{
    pldock.setVisible(visible);
}

/***************** private *******************/
/***************** slots *******************/
void ImageViewer::update_Timer_SlideShow()
{
    nextImage();
}

void ImageViewer::on_PlaylistSelected()
{
    showImage(pldock.currentIndex());
}

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

    if (event->dropAction() == Qt::CopyAction)
    {
        pldock.clear();
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
                pldock.append(path);
            }
        }
    }
    showImage(pldock.currentIndex());
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
    if (! pldock.validIndex(n))
    {
        return false;
    }

    QImage *img = new QImage(pldock.getFilePath());

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
                pldock.append(connectFilePath(path, *iterator));
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
