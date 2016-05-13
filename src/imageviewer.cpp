#include "imageviewer.hpp"
#include "image.hpp"
#include <algorithm>
#include <QScrollBar>
#include <QMimeData>

ImageViewer::ImageViewer(QWidget *parent)
    : QGraphicsView(parent)
    , view_scene(new QGraphicsScene())
    , view_item(new QGraphicsPixmapItem())
    , img_count(0)
    , img_orgs()
    , img_combined()
    , img_scaled()
    , scale_value(1.0)
    , vmode(FULLSIZE)
    , imode(Bilinear)
    , rightbinding(false)
{
    setScene(view_scene);
    view_scene->addItem(view_item);
}

ImageViewer::~ImageViewer()
{
    delete view_item;
    delete view_scene;
}

void
ImageViewer::showImage(const QStringList& paths)
{
    QVector<QImage> imgs;

    for (QStringList::const_iterator i = paths.constBegin();
            i != paths.constEnd(); ++i)
    {
        imgs << QImage(*i);
    }
    showImage(imgs);
    imgs.clear();
}

void
ImageViewer::showImage(const QVector<QImage> &imgs)
{
    int max_height = 0;
    int width_sum = 0;

    img_count = 0;
    img_orgs.clear();
    if (imgs.isEmpty()) return;

    for (QVector<QImage>::const_iterator i = imgs.constBegin();
            i != imgs.constEnd(); ++i)
    {
        img_count++;
        QImage temp = *i;
        max_height = std::max(max_height, temp.height());
        width_sum += temp.width();

        img_orgs << (temp.format() == QImage::Format_ARGB32 ? temp
                : temp.convertToFormat(QImage::Format_ARGB32));
    }

    img_combined = QImage(width_sum, max_height, QImage::Format_ARGB32);
    imageCombine(img_combined, img_orgs);
    imageScale(img_combined);

    setGraphicsPixmapItem(img_scaled);

    verticalScrollBar()->setSliderPosition(0);

    emit setNewImage();
}

void
ImageViewer::releaseImage()
{
    view_item->setPixmap(QPixmap());
    view_scene->setSceneRect(0.0, 0.0, 0.0, 0.0);

    img_count = 0;
    img_orgs.clear();
    img_combined = QImage();
    img_scaled = QImage();

    emit setNewImage();
}

int
ImageViewer::imageCount() const
{
    return img_count;
}

QSize
ImageViewer::getOriginalImageSize(int idx) const
{
    if (0 <= idx && idx < imageCount())
    {
        return img_orgs[idx].isNull() ? QSize()
                                      : img_orgs[idx].size();
    }
    else
    {
        return QSize();
    }
}

QSize
ImageViewer::getCombinedImageSize() const
{
    return img_combined.isNull() ? QSize() : img_combined.size();
}

qreal
ImageViewer::getScale() const
{
    return scale_value;
}

ImageViewer::ViewMode
ImageViewer::getScaleMode() const
{
    return vmode;
}

ImageViewer::InterpolationMode
ImageViewer::getInterpolationMode() const
{
    return imode;
}

bool
ImageViewer::getRightbindingMode() const
{
    return rightbinding;
}

QVector<int>
ImageViewer::histgram() const
{
    if (img_combined.isNull()) return QVector<int>();

    QVector<int> vec;
    int array[256*3];

    for (int i = 0; i < 256*3; ++i) array[i] = 0;

    const QRgb *bits = (QRgb*)img_combined.bits();
    const int len = img_combined.height() * img_combined.width();
    for (int i = 0; i < len; ++i)
    {
        const QRgb rgb = *(bits+i);
        array[qRed(rgb)  +256*0]++;
        array[qGreen(rgb)+256*1]++;
        array[qBlue(rgb) +256*2]++;
    }

    for (int i = 0; i < 256*3; ++i) vec << array[i];

    return vec;
}

void
ImageViewer::setScale(const ViewMode m, const qreal s)
{
    scale_value = s;
    setScale(m);
}

void
ImageViewer::setScale(const ViewMode m)
{
    vmode = m;
    if (!img_combined.isNull())
    {
        imageScale(img_combined);
        setGraphicsPixmapItem(img_scaled);
    }
}

void
ImageViewer::setInterpolationMode(const InterpolationMode mode)
{
    bool c = mode != imode;
    imode = mode;
    if (c) refresh();
}

void
ImageViewer::setRightbindingMode(bool b)
{
    bool c = rightbinding != b;
    rightbinding = b;
    if (c) refresh();
}

bool
ImageViewer::empty() const
{
    return img_combined.isNull();
}

void
ImageViewer::keyPressEvent(QKeyEvent *event)
{
    QGraphicsView::keyPressEvent(event);

    if (event->key() == Qt::Key_Left)
    {
        emit rightClicked();
    }
    else if (event->key() == Qt::Key_Right)
    {
        emit leftClicked();
    }
}

void
ImageViewer::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    if (!img_combined.isNull())
    {
        imageScale(img_combined);
        setGraphicsPixmapItem(img_scaled);
    }
}

void
ImageViewer::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void
ImageViewer::dragLeaveEvent(QDragLeaveEvent *event)
{
    // 何もしてないオーバーライドしているメソッドですが
    // 消すとD&Dがうまくいきません
    Q_UNUSED(event);
}

void
ImageViewer::dragMoveEvent(QDragMoveEvent *event)
{
    // 何もしてないオーバーライドしているメソッドですが
    // 消すとD&Dがうまくいきません
    Q_UNUSED(event);
}

void
ImageViewer::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();
    const QList<QUrl> urls = mime->urls();

    QStringList list;
    for (QList<QUrl>::const_iterator i = urls.constBegin();
            i != urls.constEnd(); ++i)
    {
        const QString path = i->toLocalFile();
        const QFileInfo info(path);
        if (info.exists()) list << path;
    }

    emit dropItems(list, isCopyDrop(event->keyboardModifiers()));
}

void
ImageViewer::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if (event->buttons() & Qt::LeftButton)  emit leftClicked();
    if (event->buttons() & Qt::RightButton) emit rightClicked();
}

QVector<QImage>
ImageViewer::imgvec_clone(const QVector<QImage> &src) const
{
    QVector<QImage> vec;
    const int len = src.count();
    for (int i = 0; i < len; ++i) vec << src[i];
    return vec;
}

void
ImageViewer::refresh()
{
    QVector<QImage> vec = imgvec_clone(img_orgs);
    showImage(vec);
    vec.clear();
}

void
ImageViewer::setGraphicsPixmapItem(const QImage& img)
{
    view_item->setPixmap(QPixmap::fromImage(img));
    view_scene->setSceneRect(0.0, 0.0, img.width(), img.height());
}

void
ImageViewer::imageScale(const QImage& img)
{
    qreal scale = 1.0;

    if (vmode == ImageViewer::CUSTOM_SCALE)
    {
        scale = scale_value;
    }
    else if (vmode == ImageViewer::FULLSIZE)
    {
        scale = 1.0;
    }
    else
    {
        qreal ws = 1.0, hs = 1.0;
        if (width() < img.width())
        {
            ws = ((qreal)width()) / ((qreal)img.width());
        }

        if (height() < img.height())
        {
            hs = ((qreal)height()) / ((qreal)img.height());
        }

        if (vmode == ImageViewer::FIT_WINDOW)
        {
            scale = ws > hs ? hs : ws;
        }
        else if (vmode == ImageViewer::FIT_IMAGE)
        {
            scale = ws;
        }
    }
    scale_value = scale;

    if (qFuzzyCompare(scale_value, 1.0))
    {
        img_scaled = img;
    }
    else
    {
        img_scaled = (QImage (*[])(const QImage, const qreal)){nn, bl, bc}[imode](img, scale);
    }
}

void
ImageViewer::imageCombine(QImage& img, QVector<QImage>& imgs) const
{
    QVector<QImage> vec = imgvec_clone(imgs);
    int sum_width = 0;
    QRgb *dst_bits = (QRgb*)img.bits();
    const int ww = img.width();
    const int hh = img.height();

    if (getRightbindingMode())
    {
        // reverse vector
        const int len = vec.count();
        for (int i = 0; i < len; ++i)
        {
            vec.append(vec[len-i-1]);
        }
        vec.remove(0, len);
    }

    for (QVector<QImage>::const_iterator i = vec.constBegin();
            i != vec.constEnd(); ++i)
    {
        const QImage *vimg = i;
        const int w = vimg->width();
        const int h = vimg->height();
        const int h2 = (hh - h) / 2;
        const QRgb *src_bits = (QRgb*)vimg->constBits();

        for (int y = 0; y < h2; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                *(dst_bits+x+sum_width+y*ww) = qRgba(255, 255, 255, 255);
            }
        }
        for (int y = 0 ; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                *(dst_bits+x+sum_width+(y+h2)*ww) = *(src_bits+x+y*w);
            }
        }
        for (int y = h+h2 ; y < hh; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                *(dst_bits+x+sum_width+y*ww) = qRgba(255, 255, 255, 255);
            }
        }
        sum_width += w;
    }
    vec.clear();
}

bool
ImageViewer::isCopyDrop(const Qt::KeyboardModifiers km)
{
#ifdef __APPLE__
    return (km & Qt::AltModifier) == Qt::AltModifier;
#else
    return (km & Qt::ControlModifier) == Qt::ControlModifier;
#endif
}

