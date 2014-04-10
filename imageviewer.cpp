#include "imageviewer.h"

const QString ImageViewer::extList[5] = {"jpg", "png", "jpeg", "bmp", "gif"};
const int ImageViewer::extListLen = 5;

ImageViewer::ImageViewer(QWidget *parent)
    : QGraphicsView(parent),
      view_scene(new QGraphicsScene()),
      view_item(new QGraphicsPixmapItem()),
      view_img(nullptr),
      img_scale(1.0),
      vmode(FULLSIZE),
      imode(Bilinear)
{
    setScene(view_scene);
}

ImageViewer::~ImageViewer()
{
    delete view_img;
    delete view_item;
    delete view_scene;
}

void ImageViewer::showImage(const QString &path)
{
    QImage *bmp = new QImage(path);
    QImage *img(nullptr);
    switch (imode)
    {
    case NearestNeighbor:
        img = nearest_neighbor(bmp, img_scale);
        break;
    case Bilinear:
        img = bilinear(bmp, img_scale);
        break;
    case Bicubic:
        img = bicubic(bmp, img_scale);
        break;
    case Lanczos2:
        break;
    case Lanczos3:
        break;
    default:
        break;
    }
    delete bmp;

    if (img == nullptr || img->isNull())
    {
        delete img;
        return;
    }

    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(*img));
    view_scene->addItem(item);
    view_scene->setSceneRect(0.0, 0.0, img->width(), img->height());

    delete view_img;
    delete view_item;
    view_img = img;
    view_item = item;

    QScrollBar *vScrollBar = verticalScrollBar();
    vScrollBar->setSliderPosition(0);

    setNewImage();
}

void ImageViewer::releaseImage()
{
    delete view_img;
    delete view_item;
    view_img = nullptr;
    view_item = nullptr;

    setNewImage();
}

QStringList ImageViewer::getReadableExtension() const
{
    QStringList list;
    for (int i = 0; i < extListLen; i++) list << extList[i];
    return list;
}

QSize ImageViewer::getImageSize() const
{
    return (view_img == nullptr) ? QSize(0, 0)
                                 : view_img->size();
}

qreal ImageViewer::getScale() const
{
    return img_scale;
}

ImageViewer::ViewMode ImageViewer::getScaleMode() const
{
    return vmode;
}

ImageViewer::InterpolationMode ImageViewer::getInterpolationMode() const
{
    return imode;
}

QVector<int> ImageViewer::histgram() const
{
    if (view_img == nullptr)
    {
        return QVector<int>();
    }

    QVector<int> vec;
    int array[256*3];

    for (int i(0); i < 256*3; ++i)
    {
        array[i] = 0;
    }

    QRgb *bits = (QRgb*)view_img->bits();
    int len = view_img->height() * view_img->width();
    for (int i(0); i < len; ++i)
    {
        QRgb rgb = *(bits+i);
        array[qRed(rgb)  +256*0]++;
        array[qGreen(rgb)+256*1]++;
        array[qBlue(rgb) +256*2]++;
    }

    for (int i(0); i < 256*3; ++i)
    {
        vec << array[i];
    }

    return vec;
}

ImageViewer& ImageViewer::setScale(ViewMode m, qreal s)
{
    vmode = m;
    img_scale = s;
    return *this;
}

ImageViewer& ImageViewer::setScale(ViewMode m)
{
    vmode = m;
    return *this;
}

ImageViewer& ImageViewer::setInterpolationMode(InterpolationMode mode)
{
    imode = mode;
    return *this;
}

bool ImageViewer::empty() const
{
    return view_img == nullptr;
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

QImage *ImageViewer::nearest_neighbor(const QImage *img, qreal s) const
{
    const int w(img->width());
    const int h(img->height());
    const int x1(w-1);
    const int y1(h-1);
    const int nw(w*s);
    const int nh(h*s);

    QImage *nimg(new QImage(nw, nh, img->format()));
    QRgb *nbits((QRgb*)nimg->bits());
    const QRgb *bits((QRgb*)img->bits());

    for (int y(0); y < nh; ++y)
    {
        const int y0 = std::min(int(std::floor(y/s+0.5)), y1)*w;

        for (int x(0); x < nw; ++x)
        {
            *(nbits+x) = *(bits+y0+
                           std::min(int(std::floor(x/s+0.5)), x1));
        }
        nbits += nw;
    }
    return nimg;
}

QImage *ImageViewer::bilinear(const QImage *img, qreal s) const
{
    const int w(img->width());
    const int h(img->height());
    const int nw(w*s);
    const int nh(h*s);

    QImage *nimg(new QImage(nw, nh, img->format()));
    QRgb *nbits((QRgb*)nimg->bits());
    const QRgb *bits((QRgb*)img->bits());

    // 先に[x],[x]+1,(x-[x]),([x]+1-x)を先に計算して保存しておく。
    int icache[nw*2];
    qreal dcache[nw*2];
    for (int x(0), p(0), w1(w-1); x < nw; ++x, p+=2)
    {
        const qreal x0(x/s);            //x;
        const int xg(std::floor(x0));   //[x];
        const qreal tx0(x0-xg);         //x-[x]
        const qreal tx1(1-tx0);         //[x]+1-x
        icache[p  ] = std::min(xg,w1);   // xg
        icache[p+1] = std::min(xg+1,w1); // xg+1
        dcache[p  ] = tx0;               // x-[x]
        dcache[p+1] = tx1;               // [x]+1-x
    }

    // ディジタル画像処理 CG-ARTS協会 2012年第2版の以下の式
    // I(x, y) =
    // ([x]+1-x) ([y]+1-y) f([x], [y])   +
    // ([x]+1-x) (y-[y])   f([x], [y]+1) +
    // (x-[x])   ([y]+1-y) f([x]+1, [y]) +
    // (x-[x])   (y-[y])   f([x]+1, [y]+1)

    for (int y(0), h1(h-1); y < nh; ++y)
    {
        const qreal y0(y/s);          //y
        const int yg(std::floor(y0)); //[y]

        const qreal ty0(y0-yg); //y-[y]
        const qreal ty1(1-ty0); //[y]+1-y
        const int yi0(std::min(yg,h1)*w);
        const int yi1(std::min(yg+1,h1)*w);

        for (int x(0), p(0); x < nw; ++x, p+=2)
        {
            const qreal t1(dcache[p+1]*ty1); //([x]+1-x)([y]+1-y)
            const qreal t2(dcache[p+1]*ty0); //([x]+1-x)(y-[y])
            const qreal t3(dcache[p  ]*ty1); //(x-[x])([y]+1-y)
            const qreal t4(dcache[p  ]*ty0); //(x-[x])(y-[y])

            const QRgb p00(*(bits+icache[p  ]+yi0));
            const QRgb p10(*(bits+icache[p+1]+yi0));
            const QRgb p01(*(bits+icache[p  ]+yi1));
            const QRgb p11(*(bits+icache[p+1]+yi1));

            *(nbits+x) = qRgba(
                        t1*qRed(p00)   +t2*qRed(p01)   +t3*qRed(p10)   +t4*qRed(p11),
                        t1*qGreen(p00) +t2*qGreen(p01) +t3*qGreen(p10) +t4*qGreen(p11),
                        t1*qBlue(p00)  +t2*qBlue(p01)  +t3*qBlue(p10)  +t4*qBlue(p11),
                        t1*qAlpha(p00) +t2*qAlpha(p01) +t3*qAlpha(p10) +t4*qAlpha(p11));
        }
        nbits += nw;
    }

    return nimg;
}

QImage *ImageViewer::bicubic(const QImage *img, qreal s) const
{
    const int w(img->width());
    const int h(img->height());
    const int w1(w-1);
    const int h1(h-1);
    const int nw(w*s);
    const int nh(h*s);

    QImage *nimg(new QImage(nw, nh, img->format()));
    QRgb *nbits((QRgb*)nimg->bits());
    const QRgb *bits((QRgb*)img->bits());

    qreal d1[4];
    int dr[4][4], dg[4][4], db[4][4], da[4][4];
    qreal d3[4];

    for (int y(0); y < nh; ++y)
    {
        const qreal y0(y/s);
        const int yg(y0);

        const qreal y2(y0-yg);
        d1[0] = bicubic_h(1+y2);
        d1[1] = bicubic_h(y2);
        d1[2] = bicubic_h(1-y2);
        d1[3] = bicubic_h(2-y2);

        for (int x(0); x < nw; ++x)
        {
            const qreal x0(x/s);
            const int xg(x0);

            const qreal x2(x0-xg);
            d3[0] = bicubic_h(1+x2);
            d3[1] = bicubic_h(x2);
            d3[2] = bicubic_h(1-x2);
            d3[3] = bicubic_h(2-x2);

            for (int i(0); i < 4; ++i)
            {
                for (int j(0); j < 4; ++j)
                {
                    const QRgb rgba(*(bits
                                      +(std::min(std::max(xg+j-1, 0), w1))
                                      +(std::min(std::max(yg+i-1, 0), h1)*w)));
                    dr[i][j] = qRed(rgba);
                    dg[i][j] = qGreen(rgba);
                    db[i][j] = qBlue(rgba);
                    da[i][j] = qAlpha(rgba);
                }
            }

            *(nbits+x) = qRgba(
                        std::min(std::max(bicubic_matmul(d1, dr, d3), 0), 0xFF),
                        std::min(std::max(bicubic_matmul(d1, dg, d3), 0), 0xFF),
                        std::min(std::max(bicubic_matmul(d1, db, d3), 0), 0xFF),
                        std::min(std::max(bicubic_matmul(d1, da, d3), 0), 0xFF));
        }
        nbits += nw;
    }
    return nimg;
}

/*
QImage *ImageViewer::lanczos2(const QImage *img, qreal s) const
{
}

QImage *ImageViewer::lanczos3(const QImage *img, qreal s) const
{
}
*/
