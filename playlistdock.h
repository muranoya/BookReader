#ifndef PLAYLISTDOCK_H
#define PLAYLISTDOCK_H

#include "nullptr.h"

#include <QList>
#include <QDockWidget>
#include <QListWidget>
#include <QFileInfo>
#include <QAction>

class PlaylistDock : public QDockWidget
{
    Q_OBJECT
public:
    PlaylistDock(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~PlaylistDock();

    void append(const QString& value);
    const QString& at(int i) const;
    void clear();
    QList<QString>::const_iterator constBegin() const
    { return items.constBegin(); }
    QList<QString>::const_iterator constEnd() const
    { return items.constEnd(); }
    bool contains(const QString& value) const
    { return items.contains(value); }
    int count() const { return items.count(); }
    bool empty() const { return items.empty(); }
    void insert(int i, const QString& value);
    void remove(int i);
    void remove(QListWidgetItem *i);
    void swap(int i, int j);

    QString getFileName() const;
    QString getFilePath() const;

    int currentIndex() const { return index; }
    int nextIndex();
    int previousIndex();
    bool validIndex(int index) const
    { return index >= 0 && index < count(); }

signals:
    void playlistItemOpen();
    void removePlaylistItem();

private slots:
    void m_open_triggered();
    void m_remove_triggered();
    void itemDoubleClicked(QListWidgetItem* item);

private:
    QList<QString> items;
    QListWidget listwidget;
    int index;
    QAction m_open;
    QAction m_remove;
};

#endif // PLAYLISTDOCK_H
