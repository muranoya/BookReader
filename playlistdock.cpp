#include "playlistdock.h"

PlaylistDock::PlaylistDock(QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(tr("プレイリスト"), parent, flags),
      index(-1),
      m_open(tr("開く"), this), m_remove(tr("プレイリストから消す"), this)
{
    setWidget(&listwidget);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    addAction(&m_open);
    addAction(&m_remove);

    connect(&m_open, SIGNAL(triggered()), this, SLOT(m_open_triggered()));
    connect(&m_remove, SIGNAL(triggered()), this, SLOT(m_remove_triggered()));
    connect(&listwidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));
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
    if (item == nullptr)
    {
        return;
    }

    listwidget.removeItemWidget(item);
    delete item;
    items.removeAt(i);

    if (empty())
    {
        index = -1;
    }
}

void PlaylistDock::remove(QListWidgetItem *i)
{
    if (i == nullptr)
    {
        return;
    }

    remove(listwidget.row(i));
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

void PlaylistDock::m_open_triggered()
{
    QList<QListWidgetItem*> litem = listwidget.selectedItems();
    if (litem.count() > 0)
    {
        index = listwidget.row(litem.at(0));
        playlistItemOpen();
    }
}

void PlaylistDock::m_remove_triggered()
{
    QList<QListWidgetItem*> litem = listwidget.selectedItems();
    QList<QListWidgetItem*>::const_iterator iterator;
    for (iterator = litem.constBegin(); iterator != litem.constEnd(); ++iterator)
    {
        remove(*iterator);
    }
    removePlaylistItem();
}

void PlaylistDock::itemDoubleClicked(QListWidgetItem *item)
{
    index = listwidget.row(item);
    playlistItemOpen();
}
