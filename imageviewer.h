#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include "nullptr.h"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QStringList>
#include <QGraphicsView>
#include <QDir>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QScrollBar>
#include <QTimer>

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

    /***************** 画像読み込み *******************/
    void loadFiles(const QStringList &paths);
    void loadDir(const QString &path);
    void nextImage();
    void previousImage();
    void releaseImages();

    /***************** getter *******************/
    QStringList getReadableExtension() const;
    int getImageListCount() const;
    int getImageListIndex() const;
    QString getFileName() const;
    QString getFilePath() const;
    QSize getImageSize() const;
    qreal getScale() const;
    ViewMode getScaleMode() const;

    /***************** setter *******************/
    ImageViewer& setAntiAliasing(bool b);
    ImageViewer& setScale(ViewMode m, qreal s);
    ImageViewer& setScale(ViewMode m);

    /***************** other ********************/
    bool isReadable(const QString &path) const;
    void startSlideShow();
    void stopSlideShow();
    bool playSlideShow() const;

signals:
    void imageChanged();
    void sizeChanged();

private slots:
    void update_Timer_SlideShow();

private:
    // 対応している拡張子
    static const QString extList[];
    static const int extListLen;

    QImage *view_img;
    QGraphicsScene *view_scene;
    QGraphicsPixmapItem *view_item;
    QStringList imgList;
    int showingIndex;
    qreal img_scale;
    ViewMode mode;
    QTimer *slideshow;

    /***************** event *******************/
    void resizeEvent(QResizeEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);

    /***************** util *******************/
    bool showImage(int n);
    void openDir(const QString &path);
    void setupMatrix();
    QString connectFilePath(const QString &parent, const QString &child) const;
};

#endif // IMAGEMANAGER_H
