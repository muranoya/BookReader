#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <cmath>
#include <algorithm>

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QScrollBar>
#include <QFileInfo>
#include <QVector>

/*
 * QImage::bits,QImage::scanLineは、それぞれ横方向の画素が入っており左上が原点、
 * 右がX軸正方向、下がY軸正方向と仮定している。
 * つまり、scanLine(0)は、1番上の画素の1行を指すポインタと解釈する。
 * bitsは同様に、1番上の1行の次は2行目と続く。
 */
class ImageViewer : public QGraphicsView
{
    Q_OBJECT
public:
    enum ViewMode
    {
        FULLSIZE,
        FIT_WINDOW,
        FIT_IMAGE,
        CUSTOM_SCALE,
    };
    enum InterpolationMode
    {
        NearestNeighbor,
        Bilinear,
        Bicubic,
    };

    explicit ImageViewer(QWidget *parent = 0);
    ~ImageViewer();

    void showImage(const QString &path);
    void showImage(const QImage &img);
    void releaseImage();

    QStringList getReadableExtension() const;
    QSize getOriginalImageSize() const;
    QSize getScaledImageSize() const;
    qreal getScale() const;
    ViewMode getScaleMode() const;
    InterpolationMode getInterpolationMode() const;

    QVector<int> histgram() const;

    ImageViewer& setScale(const ViewMode m, const qreal s);
    ImageViewer& setScale(const ViewMode m);
    ImageViewer& setInterpolationMode(const InterpolationMode mode);

    bool empty() const;
    bool isReadable(const QString &path) const;

signals:
    void rightClicked();
    void leftClicked();
    void dropItems(QStringList list, bool copy);
    void setNewImage();

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    // 対応している拡張子
    static const QString extList[];
    static const int extListLen;

    QGraphicsScene *view_scene;
    QGraphicsPixmapItem *view_item;
    QImage img_original;
    QImage img_scaled;
    qreal scale_value;
    ViewMode vmode;
    InterpolationMode imode;

    void setGraphicsPixmapItem(const QImage& img);
    void imageScale(const QImage& img);

    bool isCopyDrop(const Qt::KeyboardModifiers km);

    /***************** 画素補完 *******************/
    static QImage nearest_neighbor(const QImage img, const qreal s);
    static QImage bilinear(const QImage img, const qreal s);
    static QImage bicubic(const QImage img, const qreal s);
    static qreal bicubic_h(const qreal t);
    static int bicubic_matmul(const qreal d1[4], const int d2[4][4], const qreal d3[4]);
};

#endif // IMAGEMANAGER_H
