#include <algorithm>
#include <QDir>
#include <cassert>
#include "playlistdock.hpp"
#include "nullptr.hpp"

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
    , num_of_images(1)
    , slideshow_timer()
    , slideshow_interval(3000)
{
    setWidget(listwidget);

    connect(listwidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(itemDoubleClicked(QListWidgetItem*)));

    connect(&slideshow_timer, SIGNAL(timeout()),
            this, SLOT(slideshow_loop()));

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
    if (list.empty()) return;

    for (QStringList::const_iterator i = list.constBegin();
            i != list.constEnd(); ++i)
    {
        const QFileInfo info(*i);
        if (info.isFile())
        {
            QListWidgetItem *newitem =
                new QListWidgetItem(info.fileName(), listwidget);
            newitem->setData(Qt::ToolTipRole, *i);
            listwidget->addItem(newitem);
        }
        else if (level > 0)
        {
            const QFileInfoList entrylist = QDir(*i).entryInfoList();
            const int canonical_len = info.canonicalPath().length();
            QStringList newlist;
            for (QFileInfoList::const_iterator j = entrylist.constBegin();
                    j != entrylist.constEnd(); ++j)
            {
                if (canonical_len < j->canonicalPath().length())
                {
                    newlist << j->filePath();
                }
            }
            append(newlist, level-1);
        }
    }

    if (!validIndex(index) && validIndex(0))
    {
        index = 0;
        setHighlight();
    }
}

void
PlaylistDock::clear()
{
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
    assert(n > 0);
    clearHighlight();
    num_of_images = n;
    setHighlight();
}

QString
PlaylistDock::currentFileName(int idx) const
{
    assert(idx >= 0);
    const int tidx = (index + idx) % count();
    if (index >= 0 && validIndex(tidx))
    {
        return QFileInfo(currentFilePath(idx)).fileName();
    }
    return QString();
}

QStringList
PlaylistDock::currentFileNames() const
{
    QStringList list;
    const int len = std::min(count(), getNumOfImages());
    for (int i = 0; i < len; ++i)
    {
        QString str = currentFileName(i);
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
    assert(idx >= 0);
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
    const int len = std::min(count(), getNumOfImages());
    for (int i = 0; i < len; ++i)
    {
        QString str = currentFilePath(i);
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
    assert(idx >= 0);
    const int tidx = (index + idx) % count();
    return (index >= 0 && validIndex(tidx)) ? tidx : -1;
}

QStringList
PlaylistDock::nextFilePath()
{
    if (empty()) return QStringList();

    clearHighlight();
    index = (index + getNumOfImages()) % count();
    return setHighlight();
}

QStringList
PlaylistDock::previousFilePath()
{
    if (empty()) return QStringList();

    clearHighlight();
    index = (index - getNumOfImages()) % count();
    if (index < 0) index += count();
    return setHighlight();
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
    if (litem.empty()) return;

    clearHighlight();
    index = listwidget->row(litem.at(0));
    emit itemOpen(setHighlight());
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
    if (item == nullptr) return;

    clearHighlight();
    index = listwidget->row(item);
    emit itemOpen(setHighlight());
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

    connect(m_open,      SIGNAL(triggered()),
            this, SLOT(m_open_triggered()));
    connect(m_remove,    SIGNAL(triggered()),
            this, SLOT(m_remove_triggered()));
    connect(m_allremove, SIGNAL(triggered()),
            this, SLOT(m_allremove_triggered()));
    connect(m_allselect, SIGNAL(triggered()),
            this, SLOT(m_allselect_triggered()));
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
    QList<QListWidgetItem*> cs;
    const int len = std::min(count(), getNumOfImages());
    for (int i = 0; i < len; ++i)
    {
        const int tidx = (index+i)%count();
        cs << listwidget->item(tidx);
    }

    clearHighlight();
    for (QList<QListWidgetItem*>::const_iterator i = items.constBegin();
            i != items.constEnd(); ++i)
    {
        if (cs.contains(*i))
        {
            if (current == *i)
            {
                index = std::max(index-1, 0);
            }
            contains = true;
        }
        else
        {
            const int r = listwidget->row(*i);
            if (r < index) index--;
        }
        listwidget->removeItemWidget(*i);
        delete *i;
    }

    if (empty()) index = -1;
    setHighlight();

    itemRemoved(contains);
}

void
PlaylistDock::clearHighlight()
{
    const int len = std::min(count(), getNumOfImages());
    for (int i = 0; i < len; ++i)
    {
        const int tidx = (index+i)%count();
        if (validIndex(tidx))
        {
            listwidget->item(tidx)->setBackground(normalBC);
        }
    }
}

QStringList
PlaylistDock::setHighlight()
{
    QStringList list;
    const int len = std::min(count(), getNumOfImages());
    for (int i = 0; i < len; ++i)
    {
        const int tidx = (index+i)%count();
        listwidget->item(tidx)->setBackground(selectedBC);
        list << currentFilePath(i);
    }
    return list;
}

