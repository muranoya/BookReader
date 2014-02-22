#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include "nullptr.h"
#include <iostream>
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QStringList>
#include <QGraphicsView>
#include <QDir>
#include <QString>
#include <QSize>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QUrl>
#include <QMimeData>

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

    bool loadFile(const QString &path);
    bool loadDir(const QString &path);
    void nextImage();
    void previousImage();
    void releaseImages();
    QStringList getImageList();
    int getImageListCount();
    QStringList getReadableExtension();
    bool isReadable(const QString &path);
    int getImageListIndex();
    QString getShowingFileName();
    QSize getShowingImageSize();
    void setAntiAliasing(bool b);
    void setScrollHand(bool b);
    void setScale(ViewMode m, qreal s);
    void setScale(ViewMode m);
    void setRotate(qreal deg);
    qreal getScale();
    qreal getRotate();
    void setupMatrix();

private:
    QImage *view_img;
    QGraphicsScene *view_scene;
    QGraphicsPixmapItem *view_item;
    // 対応している拡張子
    static const QString extList[];
    static const int extListLen;
    QStringList imgList;
    int showingIndex;

    qreal img_scale, img_rotate;
    ViewMode mode;

    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

    bool showImage(int n);
    void openDir(const QString &path);
    QString connectFilePath(const QString &parent, const QString &child);
};

#endif // IMAGEMANAGER_H
