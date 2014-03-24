#ifndef PLAYLISTDOCK_H
#define PLAYLISTDOCK_H

#include <QList>
#include <QDockWidget>
#include <QListWidget>
#include <QFileInfo>

class PlaylistDock : public QDockWidget
{
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
    void swap(int i, int j);

    QString getFileName() const;
    QString getFilePath() const;

    int currentIndex() const
    { return index; }
    int nextIndex();
    int previousIndex();
    bool validIndex(int index) const
    { return index >= 0 && index < count(); }

private:
    QList<QString> items;
    QListWidget listwidget;
    int index;
};

#endif // PLAYLISTDOCK_H
