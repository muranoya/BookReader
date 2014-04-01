#include "imageviewer.h"

const QString ImageViewer::extList[5] = {"jpg", "png", "jpeg", "bmp", "gif"};
const int ImageViewer::extListLen = 5;

ImageViewer::ImageViewer() : QGraphicsView(),
    view_scene(), view_item(nullptr), view_img(nullptr),
    img_scale(1.0), mode(FULLSIZE)
{
    setScene(&view_scene);
}

ImageViewer::~ImageViewer()
{
    delete view_img;
    delete view_item;
}

void ImageViewer::showImage(const QString &path)
{
    QImage *img = new QImage(path);

    if (img->isNull())
    {
        delete img;
        return;
    }

    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(*img));
    view_scene.addItem(item);
    view_scene.setSceneRect(0.0, 0.0, img->width(), img->height());

    delete view_img;
    delete view_item;
    view_img = img;
    view_item = item;

    setupMatrix();
    QScrollBar *vScrollbar = verticalScrollBar();
    vScrollbar->setSliderPosition(0);
}

void ImageViewer::releaseImage()
{
    delete view_img;
    delete view_item;

    view_img = nullptr;
    view_item = nullptr;

    view_scene.setSceneRect(0.0, 0.0, 0.0, 0.0);
}

QStringList ImageViewer::getReadableExtension() const
{
    QStringList list;
    for (int i = 0; i < extListLen; i++)
    {
        list << extList[i];
    }
    return list;
}

QSize ImageViewer::getImageSize() const
{
    return (view_item == nullptr) ? QSize(0, 0)
                                  :  view_item->pixmap().size();
}

qreal ImageViewer::getScale() const
{
    return img_scale;
}

ImageViewer::ViewMode ImageViewer::getScaleMode() const
{
    return mode;
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

void ImageViewer::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    setupMatrix();
}

void ImageViewer::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void ImageViewer::dragLeaveEvent(QDragLeaveEvent *event)
{
    // 何もしてないオーバーライドしているメソッドですが、消すとD&Dがうまくいきません
    Q_UNUSED(event);
}

void ImageViewer::dragMoveEvent(QDragMoveEvent *event)
{
    // 何もしてないオーバーライドしているメソッドですが、消すとD&Dがうまくいきません
    Q_UNUSED(event);
}

void ImageViewer::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();
    QList<QUrl> urls = mime->urls();

    QStringList list;
    QList<QUrl>::const_iterator iterator;
    for (iterator = urls.constBegin(); iterator != urls.constEnd(); ++iterator)
    {
        QString path = (*iterator).toLocalFile();
        QFileInfo info(path);
        if (info.exists())
        {
            list << path;
        }
    }

    dropItems(list,
              (event->dropAction() & Qt::CopyAction) == Qt::CopyAction);
}

void ImageViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        leftClicked();
    }
    if (event->buttons() & Qt::RightButton)
    {
        rightClicked();
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
