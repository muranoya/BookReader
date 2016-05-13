#ifndef PLAYLISTDOCK_H
#define PLAYLISTDOCK_H

#include <QList>
#include <QDockWidget>
#include <QListWidget>
#include <QFileInfo>
#include <QAction>
#include <QFileInfoList>
#include <QTimer>

class PlaylistDock : public QDockWidget
{
    Q_OBJECT
public:
    PlaylistDock(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~PlaylistDock();

    void append(const QStringList& list, const int level = 0);
    void clear();
    int count() const;
    bool empty() const;
    int getNumOfImages() const;
    void setNumOfImages(int n);

    QString currentFileName(int idx) const;
    QStringList currentFileNames() const;
    QString currentFilePath(int idx) const;
    QStringList currentFilePaths() const;

    int currentIndex(int idx) const;
    QStringList nextFilePath();
    QStringList previousFilePath();

    void setSlideshowInterval(int msec);
    int getSlideshowInterval() const;
    bool isPlayingSlideshow() const;
    void startSlideshow();
    void stopSlideshow();

signals:
    // プレイリストのアイテムを開く処理を行った場合
    void itemOpen(QStringList paths);
    // プレイリストからアイテムを削除した場合
    void itemRemoved(bool currentFile);

    void slideshow_stop();
    void slideshow_change(QStringList name);

private slots:
    void m_open_triggered();
    void m_remove_triggered();
    void m_allremove_triggered();
    void m_allselect_triggered();
    void itemDoubleClicked(QListWidgetItem* item);

    void slideshow_loop();

private:
    QListWidget *listwidget;
    QAction *m_open;
    QAction *m_separator1;
    QAction *m_remove;
    QAction *m_allremove;
    QAction *m_separator2;
    QAction *m_allselect;
    QBrush normalBC;
    QBrush selectedBC;
    int index;
    int num_of_images;

    QTimer slideshow_timer;
    int slideshow_interval;

    void createMenus();

    bool validIndex(int idx) const;
    void remove(QList<QListWidgetItem*> items);
    void clearHighlight();
    QStringList setHighlight();
};

#endif // PLAYLISTDOCK_H
