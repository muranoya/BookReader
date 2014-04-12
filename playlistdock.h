#ifndef PLAYLISTDOCK_H
#define PLAYLISTDOCK_H

#include "nullptr.h"
#include "util.h"

#include <QList>
#include <QDockWidget>
#include <QListWidget>
#include <QFileInfo>
#include <QAction>
#include <QDir>

class PlaylistDock : public QDockWidget
{
    Q_OBJECT
public:
    PlaylistDock(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~PlaylistDock();

    /*
     * 次のように関数を呼び出された場合
     * list = ["/Users/packman/MyPictures", "/Users/packman/wallpaper.jpg"]
     * level = 0
     * MyPicturesディレクトリ以下のファイルは読み込まないが、wallpaper.jpgを読み込む。
     *
     * level = 1の場合
     * MyPicturesディレクトリ以下のファイル(ディレクトリは読み込まない)と、wallpaper.jpgを読み込む。
     *
     * level < 0の場合
     * 再帰的に全てのフォルダとファイルを読み込む。
     */
    void append(const QStringList& list, int level = 0);
    QString at(int i) const;
    void clear();
    int count() const;
    bool empty() const;

    QString currentFileName() const;
    QString currentFilePath() const;

    int currentIndex() const;
    QString nextFilePath();
    QString previousFilePath();

signals:
    void itemOpen(QString path); // コンテキストメニューかリストアイテムのダブルクリックで開いた場合
    void itemRemoved(bool currentFile);

private slots:
    void m_open_triggered();
    void m_remove_triggered();
    void m_allremove_triggered();
    void m_allselect_triggered();
    void itemDoubleClicked(QListWidgetItem* item);

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

    void createMenus();

    bool validIndex(int index) const { return index >= 0 && index < count(); }
    void remove(QList<QListWidgetItem*> items);
};

#endif // PLAYLISTDOCK_H
