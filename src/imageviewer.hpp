#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QDockWidget>
#include <QListWidget>
#include <QAction>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QFileInfo>
#include <QVector>
#include <QTimer>
#include <QCache>
#include <QByteArray>
#include <QThread>
#include <QMutex>

class Prefetcher : public QThread
{
Q_OBJECT
public:
    Prefetcher(QCache<QString, QByteArray> *ch, QMutex *m);
    ~Prefetcher();

    void setPrefetchImage(QStringList &list);

signals:
    void prefetchFinished();

protected:
    virtual void run();

private:
    QCache<QString, QByteArray> *cache;
    QMutex *mutex;
    QStringList plist;
};

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

    explicit ImageViewer(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~ImageViewer();

    void openImages(const QStringList &path);
    void clearPlaylist();

    QVector<int> histgram() const;

    void startSlideshow();
    void stopSlideshow();
    bool isPlayingSlideshow() const;

    void setSlideshowInterval(int msec);
    int getSlideshowInterval() const;

    void setSpreadMode(bool m);
    bool isSpreadMode() const;

    void setRightbindingMode(bool m);
    bool isRightbindingMode() const;

    void setScale(ViewMode m, qreal s);
    void setScale(ViewMode m);
    qreal getScale() const;
    ViewMode getScaleMode() const;

    void setInterpolationMode(InterpolationMode mode);
    InterpolationMode getInterpolationMode() const;

    void setOpenDirLevel(int n);
    int getOpenDirLevel() const;

    void setImageCacheSize(int cost);
    int getImageCacheSize() const;

    QSize orgImageSize(int i) const;
    QSize combinedImageSize() const;

    int countShowImages() const;
    int count() const;
    bool empty() const;

    int currentIndex(int i) const;
    QString currentFileName(int i) const;
    QStringList currentFileNames() const;
    QString currentFilePath(int i) const;
    QStringList currentFilePaths() const;

    QDockWidget *playlistDock() const;

signals:
    void stoppedSlideshow();
    void changeImage();

private slots:
    void menu_open_triggered();
    void menu_remove_triggered();
    void menu_clear_triggered();

    void playlistItemDoubleClicked(QListWidgetItem *item);
    void slideshow_loop();
    void prefetcher_prefetchFinished();

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

private:
    // viewer
    QGraphicsScene *view_scene;
    QGraphicsPixmapItem *view_item;
    QVector<QImage> img_orgs; // 個々の画像
    QImage img_combined;      // 全ての画像を1枚の画像に連結したもの
    QImage img_scaled;        // 拡縮後の画像
    qreal scale_value;        // 表示倍率
    int img_count;            // 同時に表示している画像数
    ViewMode vmode;           // 表示モード
    InterpolationMode imode;  // 画素補完モード
    bool rightbinding;        // 見開き表示時に右綴じで表示するか

    void releaseImages();
    void showImages();
    void refresh();
    void setGraphicsPixmapItem(const QImage &img);
    void imageScale();
    void imageCombine(const QVector<QImage> &imgs);

    // playlist
    QDockWidget *playlistdock;
    QListWidget *playlist;
    QAction *menu_open;
    QAction *menu_sep1;
    QAction *menu_remove;
    QAction *menu_clear;
    QBrush normalBC;
    QBrush selectedBC;
    int index;
    bool spread_view;
    QTimer slideshow_timer;
    int slideshow_interval;
    int opendirlevel;

    // prefetch
    QCache<QString, QByteArray> cache;
    Prefetcher prefetcher;
    QMutex prefetch_mutex;
    QList<QListWidgetItem*> prefetch_old;
    QList<QListWidgetItem*> prefetch_now;
    const QIcon prefetched_icon;

    void createPlaylistMenus();
    void playlistItemRemove(QList<QListWidgetItem*> items);
    void openFilesAndDirs(const QStringList &paths, int level);
    void setHighlight();
    void clearHighlight();
    void nextImages();
    void previousImages();
    bool validIndex(int i) const;
    bool isCopyDrop(const Qt::KeyboardModifiers km);
    void startPrefetch();
    QByteArray *readImageData(const QString &path);
};


#endif // IMAGEMANAGER_H
