#include "playlistdock.h"

PlaylistDock::PlaylistDock(QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(tr("プレイリスト"), parent, flags),
      index(-1)
{
    setWidget(&listwidget);
}

PlaylistDock::~PlaylistDock(){}

void PlaylistDock::append(const QString &value)
{
    if (empty())
    {
        index = 0;
    }
    listwidget.addItem(value);
    items.append(value);
}

const QString& PlaylistDock::at(int i) const
{
    return items.at(i);
}

void PlaylistDock::clear()
{
    while (listwidget.count() > 0)
    {
        QListWidgetItem *item = listwidget.item(0);
        listwidget.removeItemWidget(item);
        delete item;
    }
    items.clear();
    index = -1;
}

void PlaylistDock::insert(int i, const QString &value)
{
    if (empty())
    {
        index = 0;
    }
    listwidget.insertItem(i, value);
    items.insert(i, value);
}

void PlaylistDock::remove(int i)
{
    QListWidgetItem *item = listwidget.item(i);
    listwidget.removeItemWidget(item);
    delete item;
    items.removeAt(i);

    if (empty())
    {
        index = -1;
    }
}

void PlaylistDock::swap(int i, int j)
{
    items.swap(i, j);
}

QString PlaylistDock::getFileName() const
{
    if (index >= 0 && index < count())
    {
        QFileInfo info(items.at(index));
        return info.fileName();
    }
    return QString();
}

QString PlaylistDock::getFilePath() const
{
    if (index >= 0 && index < count())
    {
        return items.at(index);
    }
    return QString();
}

int PlaylistDock::nextIndex()
{
    if (index >= 0)
    {
        index++;
        if (index >= count())
        {
            index = 0;
        }
    }
    return index;
}

int PlaylistDock::previousIndex()
{
    if (index >= 0)
    {
        index--;
        if (index < 0)
        {
            index = count()-1;
        }
    }
    return index;
}
