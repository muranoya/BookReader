#include "playlistdock.h"

PlaylistDock::PlaylistDock(QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(tr("プレイリスト"), parent, flags),
      listwidget(),
      m_open(tr("開く"), this), m_remove(tr("プレイリストから消す"), this),
      normalBC(Qt::transparent), selectedBC(Qt::lightGray),
      index(-1)
{
    setWidget(&listwidget);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    addAction(&m_open);
    addAction(&m_remove);

    connect(&m_open, SIGNAL(triggered()), this, SLOT(m_open_triggered()));
    connect(&m_remove, SIGNAL(triggered()), this, SLOT(m_remove_triggered()));
    connect(&listwidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));

    listwidget.setDefaultDropAction(Qt::MoveAction);
    listwidget.setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void PlaylistDock::append(const QStringList &list, int level)
{
    if (list.empty())
    {
        return;
    }

    QStringList::const_iterator iterator;
    for (iterator = list.constBegin(); iterator != list.constEnd(); ++iterator)
    {
        QFileInfo info(*iterator);
        if (info.isFile())
        {
            QListWidgetItem *newitem = new QListWidgetItem(info.fileName(), &listwidget);
            newitem->setData(Qt::ToolTipRole, *iterator);
            listwidget.addItem(newitem);
        }
        else if (level != 0)
        {
            QDir dir(*iterator);
            QStringList entrylist = dir.entryList();
            QStringList newlist;
            QStringList::const_iterator iterator2;
            for (iterator2 = entrylist.constBegin(); iterator2 != entrylist.constEnd(); ++iterator2)
            {
                newlist << BookReader::Util::connectFilePath(*iterator, *iterator2);
            }
            append(newlist, (level < 0 ? level : level-1));
        }
    }

    if (!validIndex(index) && validIndex(0))
    {
        index = 0;
        listwidget.item(index)->setBackground(selectedBC);
    }
}

QString PlaylistDock::at(int i) const
{
    return listwidget.item(i)->data(Qt::ToolTipRole).toString();
}

void PlaylistDock::clear()
{
    while (listwidget.count() > 0)
    {
        QListWidgetItem *item = listwidget.item(0);
        listwidget.removeItemWidget(item);
        delete item;
    }
    index = -1;
}

int PlaylistDock::count() const
{
    return listwidget.count();
}

bool PlaylistDock::empty() const
{
    return count() == 0;
}

QString PlaylistDock::currentFileName() const
{
    if (validIndex(index))
    {
        QFileInfo info(currentFilePath());
        return info.fileName();
    }
    return QString();
}

QString PlaylistDock::currentFilePath() const
{
    if (validIndex(index))
    {
        return listwidget.item(index)->data(Qt::ToolTipRole).toString();
    }
    return QString();
}

int PlaylistDock::currentIndex() const
{
    return index;
}

QString PlaylistDock::nextFilePath()
{
    if (empty()) return QString();

    if (validIndex(index))
    {
        listwidget.item(index)->setBackground(normalBC);
    }

    index++;
    if (!validIndex(index))
    {
        index = 0;
    }
    listwidget.item(index)->setBackground(selectedBC);
    return currentFilePath();
}

QString PlaylistDock::previousFilePath()
{
    if (empty()) return QString();

    if (validIndex(index))
    {
        listwidget.item(index)->setBackground(normalBC);
    }

    index--;
    if (!validIndex(index))
    {
        index = count() - 1;
    }
    listwidget.item(index)->setBackground(selectedBC);
    return currentFilePath();
}

void PlaylistDock::m_open_triggered()
{
    QList<QListWidgetItem*> litem = listwidget.selectedItems();
    if (!litem.empty())
    {
        if (validIndex(index))
        {
            listwidget.item(index)->setBackground(normalBC);
        }
        index = listwidget.row(litem.at(0));
        litem.at(0)->setBackground(selectedBC);
        itemOpen(currentFilePath());
    }
}

void PlaylistDock::m_remove_triggered()
{
    remove(listwidget.selectedItems());
}

void PlaylistDock::itemDoubleClicked(QListWidgetItem *item)
{
    if (validIndex(index))
    {
        listwidget.item(index)->setBackground(normalBC);
    }
    index = listwidget.row(item);
    item->setBackground(selectedBC);
    itemOpen(currentFilePath());
}

void PlaylistDock::remove(QList<QListWidgetItem*> items)
{
    bool contains = false;
    QListWidgetItem *current = validIndex(index) ? listwidget.item(index) : nullptr;

    QList<QListWidgetItem*>::const_iterator iterator;
    for (iterator = items.constBegin(); iterator != items.constEnd(); ++iterator)
    {
        if (current == *iterator)
        {
            listwidget.removeItemWidget(*iterator);
            delete *iterator;
            --index;
            nextFilePath();
            contains = true;
        }
        else
        {
            int r = listwidget.row(*iterator);
            if (r < index)
            {
                --index;
            }
            listwidget.removeItemWidget(*iterator);
            delete *iterator;
        }
    }

    if (empty())
    {
        index = -1;
    }
    else if (validIndex(index))
    {
        listwidget.item(index)->setBackground(selectedBC);
    }

    itemRemoved(contains);
}
