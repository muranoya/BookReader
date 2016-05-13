#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QFileInfo>
#include <QVector>

/*
 * QImage::bits,QImage::scanLineは、それぞれ横方向の画素が
 * 入っており左上が原点、
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

    void showImage(const QStringList& paths);
    void showImage(const QVector<QImage>& imgs);
    void releaseImage();

    int imageCount() const;

    QSize getOriginalImageSize(int idx) const;
    QSize getCombinedImageSize() const;
    qreal getScale() const;
    ViewMode getScaleMode() const;
    InterpolationMode getInterpolationMode() const;
    bool getRightbindingMode() const;

    QVector<int> histgram() const;

    void setScale(const ViewMode m, const qreal s);
    void setScale(const ViewMode m);
    void setInterpolationMode(const InterpolationMode mode);
    void setRightbindingMode(bool b);

    bool empty() const;

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
    QGraphicsScene *view_scene;
    QGraphicsPixmapItem *view_item;
    // 同時に表示する画像数
    int img_count;
    // 個々の画像
    QVector<QImage> img_orgs;
    // 全ての画像を1枚の画像に連結したもの
    QImage img_combined;
    // 拡縮後の画像
    QImage img_scaled;
    qreal scale_value;
    ViewMode vmode;
    InterpolationMode imode;
    bool rightbinding;

    QVector<QImage> imgvec_clone(const QVector<QImage> &src) const;
    void refresh();
    void setGraphicsPixmapItem(const QImage& img);
    void imageScale(const QImage& img);
    void imageCombine(QImage& img, QVector<QImage>& imgs) const;
    bool isCopyDrop(const Qt::KeyboardModifiers km);
};

#endif // IMAGEMANAGER_H
