#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include "nullptr.h"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QScrollBar>
#include <QFileInfo>

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

    ImageViewer();
    ~ImageViewer();

    void showImage(const QString &path);
    void releaseImage();

    QStringList getReadableExtension() const;
    QSize getImageSize() const;
    qreal getScale() const;
    ViewMode getScaleMode() const;

    ImageViewer& setScale(ViewMode m, qreal s);
    ImageViewer& setScale(ViewMode m);

    bool isReadable(const QString &path) const;

signals:
    void rightClicked();
    void leftClicked();
    void dropItems(QStringList list, bool copy);

private:
    // 対応している拡張子
    static const QString extList[];
    static const int extListLen;

    QGraphicsScene view_scene;
    QGraphicsPixmapItem *view_item;
    QImage *view_img;
    qreal img_scale;
    ViewMode mode;

    /***************** event *******************/
    void resizeEvent(QResizeEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void setupMatrix();
};

#endif // IMAGEMANAGER_H
