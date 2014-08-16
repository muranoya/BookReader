#include "playlistdock.hpp"

PlaylistDock::PlaylistDock(QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(tr("プレイリスト"), parent, flags)
    , listwidget(new QListWidget())
    , m_open(nullptr)
    , m_separator1(nullptr)
    , m_remove(nullptr)
    , m_allremove(nullptr)
    , m_separator2(nullptr)
    , m_allselect(nullptr)
    , normalBC(Qt::transparent)
    , selectedBC(Qt::lightGray)
    , index(-1)
{
    setWidget(listwidget);

    connect(listwidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));

    listwidget->setDefaultDropAction(Qt::MoveAction);
    listwidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    createMenus();
}

PlaylistDock::~PlaylistDock()
{
    delete listwidget;
    delete m_open;
    delete m_separator1;
    delete m_remove;
    delete m_allremove;
    delete m_separator2;
    delete m_allselect;
}

void
PlaylistDock::append(const QStringList &list, const int level)
{
    if (list.empty())
    {
        return;
    }

    QStringList::const_iterator iterator;
    for (iterator = list.constBegin(); iterator != list.constEnd(); ++iterator)
    {
        const QFileInfo info(*iterator);
        if (info.isFile())
        {
            QListWidgetItem *newitem = new QListWidgetItem(info.fileName(), listwidget);
            newitem->setData(Qt::ToolTipRole, *iterator);
            listwidget->addItem(newitem);
        }
        else if (level != 0)
        {
            const QDir dir(*iterator);
            const QFileInfoList entrylist = dir.entryInfoList();
            QStringList newlist;
            QFileInfoList::const_iterator iterator2;
            for (iterator2 = entrylist.constBegin(); iterator2 != entrylist.constEnd(); ++iterator2)
            {
                newlist << iterator2->filePath();
            }
            append(newlist, (level < 0 ? level : level-1));
        }
    }

    if (!validIndex(index) && validIndex(0))
    {
        index = 0;
        listwidget->item(index)->setBackground(selectedBC);
    }
}

QString
PlaylistDock::at(const int i) const
{
    return listwidget->item(i)->data(Qt::ToolTipRole).toString();
}

void
PlaylistDock::clear()
{
    const bool contain = !empty();
    while (listwidget->count() > 0)
    {
        QListWidgetItem *item = listwidget->item(0);
        listwidget->removeItemWidget(item);
        delete item;
    }
    index = -1;

    itemRemoved(contain);
}

int
PlaylistDock::count() const
{
    return listwidget->count();
}

bool
PlaylistDock::empty() const
{
    return count() == 0;
}

QString
PlaylistDock::currentFileName() const
{
    if (validIndex(index))
    {
        const QFileInfo info(currentFilePath());
        return info.fileName();
    }
    return QString();
}

QString
PlaylistDock::currentFilePath() const
{
    if (validIndex(index))
    {
        return listwidget->item(index)->data(Qt::ToolTipRole).toString();
    }
    return QString();
}

int
PlaylistDock::currentIndex() const
{
    return index;
}

QString
PlaylistDock::nextFilePath()
{
    if (empty())
    {
        return QString();
    }

    if (validIndex(index))
    {
        listwidget->item(index)->setBackground(normalBC);
    }

    index++;
    if (!validIndex(index))
    {
        index = 0;
    }
    listwidget->item(index)->setBackground(selectedBC);
    return currentFilePath();
}

QString
PlaylistDock::previousFilePath()
{
    if (empty())
    {
        return QString();
    }

    if (validIndex(index))
    {
        listwidget->item(index)->setBackground(normalBC);
    }

    index--;
    if (!validIndex(index))
    {
        index = count() - 1;
    }
    listwidget->item(index)->setBackground(selectedBC);
    return currentFilePath();
}

void
PlaylistDock::m_open_triggered()
{
    QList<QListWidgetItem*> litem = listwidget->selectedItems();
    if (!litem.empty())
    {
        if (validIndex(index))
        {
            listwidget->item(index)->setBackground(normalBC);
        }
        index = listwidget->row(litem.at(0));
        litem.at(0)->setBackground(selectedBC);
        emit itemOpen(currentFilePath());
    }
}

void
PlaylistDock::m_remove_triggered()
{
    remove(listwidget->selectedItems());
}

void
PlaylistDock::m_allremove_triggered()
{
    clear();
}

void
PlaylistDock::m_allselect_triggered()
{
    const int size = count();
    for (int i = 0; i < size; ++i)
    {
        listwidget->item(i)->setSelected(true);
    }
}

void
PlaylistDock::itemDoubleClicked(QListWidgetItem *item)
{
    if (validIndex(index))
    {
        listwidget->item(index)->setBackground(normalBC);
    }
    index = listwidget->row(item);
    item->setBackground(selectedBC);
    emit itemOpen(currentFilePath());
}

void
PlaylistDock::createMenus()
{
    setContextMenuPolicy(Qt::ActionsContextMenu);

    m_separator1 = new QAction(this);
    m_separator1->setSeparator(true);
    m_separator2 = new QAction(this);
    m_separator2->setSeparator(true);

    m_open = new QAction(tr("開く"), this);
    m_remove = new QAction(tr("削除する"), this);
    m_allremove = new QAction(tr("全て削除する"), this);
    m_allselect = new QAction(tr("全て選択する"), this);

    addAction(m_open);
    addAction(m_separator1);
    addAction(m_remove);
    addAction(m_allremove);
    addAction(m_separator2);
    addAction(m_allselect);

    connect(m_open ,SIGNAL(triggered()), this, SLOT(m_open_triggered()));
    connect(m_remove, SIGNAL(triggered()), this, SLOT(m_remove_triggered()));
    connect(m_allremove, SIGNAL(triggered()), this, SLOT(m_allremove_triggered()));
    connect(m_allselect, SIGNAL(triggered()), this, SLOT(m_allselect_triggered()));
}

bool
PlaylistDock::validIndex(int index) const
{
     return index >= 0 && index < count();
}

void
PlaylistDock::remove(QList<QListWidgetItem*> items)
{
    bool contains = false;
    QListWidgetItem *current = validIndex(index) ? listwidget->item(index) : nullptr;

    QList<QListWidgetItem*>::const_iterator iterator;
    for (iterator = items.constBegin(); iterator != items.constEnd(); ++iterator)
    {
        if (current == *iterator)
        {
            listwidget->removeItemWidget(*iterator);
            delete *iterator;
            --index;
            nextFilePath();
            contains = true;
        }
        else
        {
            const int r = listwidget->row(*iterator);
            if (r < index)
            {
                --index;
            }
            listwidget->removeItemWidget(*iterator);
            delete *iterator;
        }
    }

    if (empty())
    {
        index = -1;
    }
    else if (validIndex(index))
    {
        listwidget->item(index)->setBackground(selectedBC);
    }

    itemRemoved(contains);
}
