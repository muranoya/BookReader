#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include "nullptr.h"
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QStringList>
#include <QGraphicsView>
#include <QDir>
#include <QString>
#include <QSize>

class ImageManager
{
public:
    ImageManager(QGraphicsView *view);
    ~ImageManager();

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

private:
    QGraphicsView *view;
    QImage *view_img;
    QGraphicsScene *view_gscene;
    QGraphicsPixmapItem *view_gitem;
    // 対応している拡張子
    QString extList[5] = {"jpg", "png", "jpeg", "bmp", "gif"};
    const int extListLen = 5;
    QStringList imgList;
    int showingIndex;

    bool showImage(int n);
    QString connectFilePath(QString parent, QString child);
};

#endif // IMAGEMANAGER_H
