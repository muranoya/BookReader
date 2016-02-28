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
    , num_of_images(2)
    , slideshow_timer()
    , slideshow_interval(3000)
{
    setWidget(listwidget);

    connect(listwidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoubleClicked(QListWidgetItem*)));

    connect(&slideshow_timer, SIGNAL(timeout()), this, SLOT(slideshow_loop()));

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
        else if (level > 0)
        {
            const QDir dir(*iterator);
            const QFileInfoList entrylist = dir.entryInfoList();
            QStringList newlist;
            QFileInfoList::const_iterator iterator2;
            int canonical_len = info.canonicalPath().length();
            for (iterator2 = entrylist.constBegin(); iterator2 != entrylist.constEnd(); ++iterator2)
            {
                if (canonical_len < (*iterator2).canonicalPath().length())
                {
                    newlist << iterator2->filePath();
                }
            }
            append(newlist, level-1);
        }
    }

    if (!validIndex(index) && validIndex(0))
    {
        index = 0;
        int tidx;
        const int len = getNumOfImages();
        for (int i = 0; i < len; ++i)
        {
            tidx = (index + i) % count();
            listwidget->item(tidx)->setBackground(selectedBC);
        }
    }
}

void
PlaylistDock::clear()
{
    const bool contain = !empty();
    QListWidgetItem *item;
    while (listwidget->count() > 0)
    {
        item = listwidget->item(0);
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
    return (count() == 0);
}

int
PlaylistDock::getNumOfImages() const
{
    return num_of_images;
}

void
PlaylistDock::setNumOfImages(int n)
{
    num_of_images = n;
}

QString
PlaylistDock::currentFileName(int idx) const
{
    const int tidx = (index + idx) % count();
    if (index >= 0 && validIndex(tidx))
    {
        const QFileInfo info(currentFilePath(idx));
        return info.fileName();
    }
    return QString();
}

QStringList
PlaylistDock::currentFileNames() const
{
    QStringList list;
    QString str;
    const int len = getNumOfImages();
    for (int i = 0; i < len; ++i)
    {
        str = currentFileName(i);
        if (str.isEmpty())
        {
            break;
        }
        else
        {
            list << str;
        }
    }
    return list;
}

QString
PlaylistDock::currentFilePath(int idx) const
{
    const int tidx = (index + idx) % count();
    if (index >= 0 && validIndex(tidx))
    {
        return listwidget->item(tidx)->data(Qt::ToolTipRole).toString();
    }
    return QString();
}

QStringList
PlaylistDock::currentFilePaths() const
{
    QStringList list;
    QString str;
    const int len = getNumOfImages();
    for (int i = 0; i < len; ++i)
    {
        str = currentFilePath(i);
        if (str.isEmpty())
        {
            break;
        }
        else
        {
            list << str;
        }
    }
    return list;
}

int
PlaylistDock::currentIndex(int idx) const
{
    const int tidx = (index + idx) % count();
    return (index >= 0 && validIndex(tidx)) ? tidx : -1;
}

QStringList
PlaylistDock::nextFilePath()
{
    QStringList list;
    if (empty())
    {
        return list;
    }

    int tidx;
    const int len = getNumOfImages();
    for (int i = 0; i < len; ++i)
    {
        tidx = (index + i) % count();
        if (validIndex(tidx))
        {
            listwidget->item(tidx)->setBackground(normalBC);
        }
    }

    index = (index + num_of_images) % count();
    for (int i = 0; i < len; ++i)
    {
        tidx = (index + i) % count();
        listwidget->item(tidx)->setBackground(selectedBC);
        list << currentFilePath(i);
    }
    return list;
}

QStringList
PlaylistDock::previousFilePath()
{
    QStringList list;
    if (empty())
    {
        return list;
    }

    int tidx;
    const int len = getNumOfImages();
    for (int i = 0; i < len; ++i)
    {
        tidx = (index + i) % count();
        if (validIndex(tidx))
        {
            listwidget->item(tidx)->setBackground(normalBC);
        }
    }

    index = (index - num_of_images) % count();
    if (index < 0)
    {
        index += count();
    }

    for (int i = 0; i < len; ++i)
    {
        tidx = (index + i) % count();
        listwidget->item(tidx)->setBackground(selectedBC);
        list << currentFilePath(i);
    }
    return list;
}

void
PlaylistDock::setSlideshowInterval(int msec)
{
    slideshow_interval = msec;
}

int
PlaylistDock::getSlideshowInterval() const
{
    return slideshow_interval;
}

bool
PlaylistDock::isPlayingSlideshow() const
{
    return slideshow_timer.isActive();
}

void
PlaylistDock::startSlideshow()
{
    slideshow_timer.start(slideshow_interval);
}

void
PlaylistDock::stopSlideshow()
{
    slideshow_timer.stop();
    emit slideshow_stop();
}

void
PlaylistDock::m_open_triggered()
{
    QList<QListWidgetItem*> litem = listwidget->selectedItems();
    if (litem.empty())
    {
        return;
    }

    int tidx;
    const int len = getNumOfImages();
    if (validIndex(index))
    {
        for (int i = 0; i < len; ++i)
        {
            tidx = (index + i) % count();
            listwidget->item(tidx)->setBackground(normalBC);
        }
    }

    index = listwidget->row(litem.at(0));
    for (int i = 0; i < len; ++i)
    {
        tidx = (index + i) % count();
        listwidget->item(tidx)->setBackground(selectedBC);
    }

    emit itemOpen(currentFilePaths());
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
    if (item == nullptr)
    {
        return;
    }

    int tidx;
    const int len = getNumOfImages();
    if (validIndex(index))
    {
        for (int i = 0; i < len; ++i)
        {
            tidx = (index + i) % count();
            listwidget->item(tidx)->setBackground(normalBC);
        }
    }

    index = listwidget->row(item);
    for (int i = 0; i < len; ++i)
    {
        tidx = (index + i) % count();
        listwidget->item(tidx)->setBackground(selectedBC);
    }

    emit itemOpen(currentFilePaths());
}

void
PlaylistDock::slideshow_loop()
{
    if (empty())
    {
        stopSlideshow();
    }
    else
    {
        emit slideshow_change(nextFilePath());
    }
}

void
PlaylistDock::createMenus()
{
    setContextMenuPolicy(Qt::ActionsContextMenu);

    m_separator1 = new QAction(this);
    m_separator1->setSeparator(true);
    m_separator2 = new QAction(this);
    m_separator2->setSeparator(true);

    m_open = new QAction(tr("表示する"), this);
    m_remove = new QAction(tr("削除する"), this);
    m_allremove = new QAction(tr("全て削除する"), this);
    m_allselect = new QAction(tr("全て選択する"), this);

    addAction(m_open);
    addAction(m_separator1);
    addAction(m_remove);
    addAction(m_allremove);
    addAction(m_separator2);
    addAction(m_allselect);

    connect(m_open,      SIGNAL(triggered()), this, SLOT(m_open_triggered()));
    connect(m_remove,    SIGNAL(triggered()), this, SLOT(m_remove_triggered()));
    connect(m_allremove, SIGNAL(triggered()), this, SLOT(m_allremove_triggered()));
    connect(m_allselect, SIGNAL(triggered()), this, SLOT(m_allselect_triggered()));
}

bool
PlaylistDock::validIndex(int idx) const
{
     return (0 <= idx && idx < count());
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
        int tidx;
        const int len = getNumOfImages();
        for (int i = 0; i < len; ++i)
        {
            tidx = (index + i) % count();
            listwidget->item(tidx)->setBackground(selectedBC);
        }
    }

    itemRemoved(contains);
}

