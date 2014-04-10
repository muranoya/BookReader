#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include "nullptr.h"

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
        Lanczos2,
        Lanczos3,
    };

    explicit ImageViewer(QWidget *parent = 0);
    ~ImageViewer();

    void showImage(const QString &path);
    void releaseImage();

    QStringList getReadableExtension() const;
    QSize getImageSize() const;
    qreal getScale() const;
    ViewMode getScaleMode() const;
    InterpolationMode getInterpolationMode() const;

    QVector<int> histgram() const;

    ImageViewer& setScale(ViewMode m, qreal s);
    ImageViewer& setScale(ViewMode m);
    ImageViewer& setInterpolationMode(InterpolationMode mode);

    bool empty() const;
    bool isReadable(const QString &path) const;

signals:
    void rightClicked();
    void leftClicked();
    void dropItems(QStringList list, bool copy);
    void setNewImage();

protected:
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
    QImage *view_img;
    qreal img_scale;
    ViewMode vmode;
    InterpolationMode imode;

    /***************** 画素補完 *******************/
    QImage *nearest_neighbor(const QImage *img, qreal s) const;
    QImage *bilinear(const QImage *img, qreal s) const;
    QImage *bicubic(const QImage *img, qreal s) const;
    inline qreal bicubic_h(qreal t) const
    {
        const qreal u = std::fabs(t);
        if (u <= 1)
            return (u*u*u)-2*(u*u)+1; // (a+2)|t|^3 -(a+3)|t|^2 +1 |t|<=1のとき
        else if (1 < u && u <= 2)
            return -(u*u*u)+5*(u*u)-8*u+4; // a|t|^3 -5a|t|^2 +8a|t| -4a 1<t<=2のとき
        return 0; // 0 2<|t|のとき
    }
    int bicubic_matmul(const qreal d1[4], const int d2[4][4], const qreal d3[4]) const
    {
        qreal temp[4];
        for (int i(0); i < 4; ++i)
        {
            temp[i] = d1[0]*d2[0][i]
                    + d1[1]*d2[1][i]
                    + d1[2]*d2[2][i]
                    + d1[3]*d2[3][i];
        }
        return int(temp[0]*d3[0]
                  +temp[1]*d3[1]
                  +temp[2]*d3[2]
                  +temp[3]*d3[3]);
    }
    /*
    QImage *lanczos2(const QImage *img, qreal s) const;
    QImage *lanczos3(const QImage *img, qreal s) const;
    */
};

#endif // IMAGEMANAGER_H
