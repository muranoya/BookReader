#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

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

class ImageViewer : public QWidget
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
    enum FeedPageMode
    {
        MouseClickPosition,
        MouseButton,
    };

    explicit ImageViewer(QWidget *parent = 0);
    ~ImageViewer();

    void openImages(const QStringList &path);
    void clearPlaylist();

    void setSpreadMode(bool m);
    bool isSpreadMode() const;

    void setRightbindingMode(bool m);
    bool isRightbindingMode() const;

    void setFeedPageMode(FeedPageMode m);
    FeedPageMode getFeedPageMode() const;

    bool setScale(ViewMode m, double s);
    bool setScale(ViewMode m);
    double getScale() const;
    ViewMode getScaleMode() const;

    void setInterpolationMode(InterpolationMode mode);
    InterpolationMode getInterpolationMode() const;

    void setOpenDirLevel(int n);
    int getOpenDirLevel() const;

    void setImageCacheSize(int cost);
    int getImageCacheSize() const;

    QSize imageSize() const;

    int countShowImages() const;
    int count() const;
    bool empty() const;

    int currentIndex(int i) const;
    QStringList currentFileNames() const;
    QString currentFileName(int i) const;

    QDockWidget *playlistDock() const;
    const QString& readableExtFormat() const;

signals:
    void changedImage();
    void changedScaleMode();

private slots:
    void menu_open_triggered();
    void menu_remove_triggered();
    void menu_clear_triggered();
    void menu_enc_triggered();

    void playlistItemDoubleClicked(QListWidgetItem *item);
    void playlistItemSelectionChanged();
    void prefetcherFinished();
    void drag_check();

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

private:
    class File
    {
        public:
            enum FileType
            {
                INVALID,
                RAW,
                ARCHIVE,
            };
            explicit File(const QString &p,
                    const QByteArray &rfilepath);
            explicit File(const QString &p);
            explicit File();
            virtual ~File();
            FileType fileType() const;
            QString physicalFilePath() const;
            QString physicalFileName() const;
            QString logicalFilePath() const;
            QString logicalFileName() const;
            void changeTextEnc(const QTextCodec *c);
            const QByteArray& rawFilePath() const;
            QString createKey() const;
        private:
            FileType ft;
            QString archive_path;
            QString file_path;
            QByteArray raw_file_entry;
    };

    class PlayListItem : public QListWidgetItem
    {
        public:
            explicit PlayListItem(const QString &p,
                    const QByteArray &f,
                    QListWidget *parent = 0);
            explicit PlayListItem(const QString &f,
                    QListWidget *parent = 0);
            virtual ~PlayListItem();
            File &file();
            void refreshText();
        private:
            File f;
    };

    class Prefetcher : public QThread
    {
        public:
            Prefetcher(QCache<QString, QByteArray> *ch,
                    QMutex *m);
            void setPrefetchImage(const QList<File> &list);
            void sendTermSig();
        protected:
            virtual void run();
        private:
            QCache<QString, QByteArray> *cache;
            QMutex *mutex;
            QList<File> plist;
            bool termsig;
    };

    // viewer
    QImage img_combined;     // 全ての画像を1枚の画像に連結したもの
    QImage img_scaled;       // 拡縮後の画像
    double scale_value;       // 表示倍率
    int img_count;           // 同時に表示している画像数
    ViewMode vmode;          // 表示モード
    InterpolationMode imode; // 画素補完モード
    bool rightbinding;       // 見開き表示時に右綴じで表示するか
    QTimer drag_timer;
    bool is_drag_image;
    QPoint click_pos;    // クリックした時の位置
    QPoint click_pos2;
    QPoint move_pos;     // 移動した時の位置
    QPoint img_pos;      // 画像の位置
    FeedPageMode fpmode; // ページ遷移のモード

    bool isReadableImageFile(const QString &path) const;
    void releaseImages();
    void showImages();
    void refresh();
    void imageScale();
    void imageCombine(const QVector<QImage> &imgs);

    // playlist
    QDockWidget *playlistdock;
    QListWidget *playlist;
    QAction *menu_open;
    QAction *menu_sep1;
    QAction *menu_remove;
    QAction *menu_clear;
    QAction *menu_sep2;
    QAction *menu_enc;
    QBrush normalBC;
    QBrush selectedBC;
    int index;
    bool spread_view;
    int opendirlevel;

    bool validIndex(int i) const;
    void createPlaylistMenus();
    void playlistItemRemove(const QList<QListWidgetItem*> &items);
    void setHighlight();
    void clearHighlight();
    File currentFile(int i) const;
    QList<File> currentFiles() const;
    void nextImages();
    void previousImages();
    bool openArchiveFile(const QString &path);
    void openFilesAndDirs(const QStringList &paths, int level);
    static QByteArray *readImageData(const File &f);
    static QByteArray *readArchiveData(const File &f);
    QByteArray *readData(const File &f);

    // prefetch
    QCache<QString, QByteArray> cache;
    Prefetcher prfter;
    QMutex prft_mutex;
    QList<QListWidgetItem*> prft_old;
    QList<QListWidgetItem*> prft_now;
    const QIcon prft_icon;

    void startPrefetch();

    // drag-and-drop
    bool isCopyDrop(const Qt::KeyboardModifiers km) const;
};

#endif // IMAGEMANAGER_H
