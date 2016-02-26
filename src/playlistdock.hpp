#ifndef PLAYLISTDOCK_H
#define PLAYLISTDOCK_H

#include "nullptr.hpp"

#include <QList>
#include <QDockWidget>
#include <QListWidget>
#include <QFileInfo>
#include <QAction>
#include <QDir>
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

    QString currentFileName() const;
    QString currentFilePath() const;

    int currentIndex() const;
    QString nextFilePath();
    QString previousFilePath();

    void setSlideshowRepeat(bool flag);
    bool getSlideshowRepeat();
    void setSlideshowInterval(int msec);
    int getSlideshowInterval();
    bool isPlayingSlideshow();
    void startSlideshow();
    void stopSlideshow();

signals:
    void itemOpen(QString path); // コンテキストメニューかリストアイテムのダブルクリックで開いた場合
    void itemRemoved(bool currentFile);

    void slideshow_stop();
    void slideshow_change(QString name);

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

    QTimer slideshow_timer;
    bool slideshow_repeat;
    int slideshow_start_index;
    int slideshow_interval;

    void createMenus();

    bool validIndex(int index) const;
    void remove(QList<QListWidgetItem*> items);
};

#endif // PLAYLISTDOCK_H
