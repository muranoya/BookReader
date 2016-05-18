#include "imageviewer.hpp"
#include "image.hpp"
#include "nullptr.hpp"

#include <algorithm>
#include <QScrollBar>
#include <QMimeData>
#include <QDir>
#include <cassert>

ImageViewer::ImageViewer(QWidget *parent, Qt::WindowFlags flags)
    : QGraphicsView(parent)
    , view_scene(new QGraphicsScene())
    , view_item(new QGraphicsPixmapItem())
    , img_orgs()
    , img_combined()
    , img_scaled()
    , scale_value(1.0)
    , img_count(0)
    , vmode(FULLSIZE)
    , imode(Bilinear)
    , rightbinding(false)
    , drag_timer()
    , is_drag_image(false)
    , click_pos()
    , move_pos()
    /* playlist */
    , playlistdock(new QDockWidget(tr("プレイリスト"), parent, flags))
    , playlist(new QListWidget())
    , menu_open(nullptr)
    , menu_sep1(nullptr)
    , menu_remove(nullptr)
    , menu_clear(nullptr)
    , normalBC(Qt::transparent)
    , selectedBC(Qt::lightGray)
    , index(-1)
    , spread_view(false)
    , slideshow_timer()
    , slideshow_interval(3000)
    , opendirlevel(30)
    , cache(10)
    , prefetcher(&cache, &prefetch_mutex)
    , prefetch_mutex()
    , prefetch_old()
    , prefetch_now()
    , prefetched_icon(":/check.png")
{
    setScene(view_scene);
    view_scene->addItem(view_item);
    playlistdock->setWidget(playlist);

    createPlaylistMenus();

    connect(playlist, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(playlistItemDoubleClicked(QListWidgetItem*)));
    connect(&slideshow_timer, SIGNAL(timeout()),
            this, SLOT(slideshow_loop()));
    connect(&drag_timer, SIGNAL(timeout()),
            this, SLOT(drag_check()));
    connect(&prefetcher, SIGNAL(prefetchFinished()),
            this, SLOT(prefetcher_prefetchFinished()),
            Qt::QueuedConnection);

    playlist->setDefaultDropAction(Qt::MoveAction);
    playlist->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

ImageViewer::~ImageViewer()
{
    delete view_item;
    delete view_scene;

    delete menu_open;
    delete menu_sep1;
    delete menu_remove;
    delete menu_clear;
    delete playlist;
    delete playlistdock;
}

void
ImageViewer::openImages(const QStringList &path)
{
    bool req_refresh =
        countShowImages() == 0 ||
        (isRightbindingMode() && countShowImages() != 2);

    clearHighlight();
    openFilesAndDirs(path, getOpenDirLevel());
    setHighlight();

    if (req_refresh && countShowImages() > 0)
    {
        index = 0;
        showImages();
    }
}

void
ImageViewer::clearPlaylist()
{
    bool c = countShowImages() > 0;
    while (playlist->count() > 0)
    {
        QListWidgetItem *item = playlist->item(0);
        playlist->removeItemWidget(item);
        delete item;
    }
    releaseImages();
    if (c) emit changeImage();
}

QVector<int>
ImageViewer::histgram() const
{
    if (img_combined.isNull()) return QVector<int>();
    QVector<int> vec(256*3);

    int *array = vec.data();
    for (int i = 0; i < 256*3; ++i) array[i] = 0;

    const QRgb *bits = (QRgb*)img_combined.bits();
    const int len = img_combined.height() * img_combined.width();
    for (int i = 0; i < len; ++i)
    {
        const QRgb rgb = *(bits+i);
        array[qRed(rgb)  +256*0]++;
        array[qGreen(rgb)+256*1]++;
        array[qBlue(rgb) +256*2]++;
    }
    return vec;
}

void
ImageViewer::startSlideshow()
{
    if (!isPlayingSlideshow())
    {
        slideshow_timer.start(slideshow_interval);
    }
}

void
ImageViewer::stopSlideshow()
{
    if (isPlayingSlideshow())
    {
        slideshow_timer.stop();
        emit stoppedSlideshow();
    }
}

bool
ImageViewer::isPlayingSlideshow() const
{
    return slideshow_timer.isActive();
}

void
ImageViewer::setSlideshowInterval(int msec)
{
    slideshow_interval = msec;
}

int
ImageViewer::getSlideshowInterval() const
{
    return slideshow_interval;
}

void
ImageViewer::setSpreadMode(bool m)
{
    if (m != spread_view)
    {
        clearHighlight();
        spread_view = m;
        setHighlight();
        showImages();
    }
}

bool
ImageViewer::isSpreadMode() const
{
    return spread_view;
}

void
ImageViewer::setRightbindingMode(bool m)
{
    if (m != rightbinding)
    {
        rightbinding = m;
        if (!empty()) showImages();
    }
}

bool
ImageViewer::isRightbindingMode() const
{
    return rightbinding;
}

void
ImageViewer::setScale(ViewMode m, qreal s)
{
    bool c = m != vmode || s != scale_value;
    vmode = m;
    scale_value = s;

    if (vmode == FULLSIZE || vmode == CUSTOM_SCALE)
    {
        setDragMode(QGraphicsView::ScrollHandDrag);
    }
    else
    {
        setDragMode(QGraphicsView::NoDrag);
    }
    if (c) refresh();
}

void
ImageViewer::setScale(ViewMode m)
{
    setScale(m, scale_value);
}

qreal
ImageViewer::getScale() const
{
    return scale_value;
}

ImageViewer::ViewMode
ImageViewer::getScaleMode() const
{
    return vmode;
}

void
ImageViewer::setInterpolationMode(InterpolationMode m)
{
    bool c = m != imode;
    imode = m;
    if (c) refresh();
}

ImageViewer::InterpolationMode
ImageViewer::getInterpolationMode() const
{
    return imode;
}

void
ImageViewer::setOpenDirLevel(int n)
{
    assert(n >= 0);
    opendirlevel = n;
}

int
ImageViewer::getOpenDirLevel() const
{
    return opendirlevel;
}

void
ImageViewer::setImageCacheSize(int cost)
{
    assert(cost >= 0);
    prefetch_mutex.lock();
    cache.setMaxCost(cost);
    prefetch_mutex.unlock();
    
    startPrefetch();
}

int
ImageViewer::getImageCacheSize() const
{
    return cache.maxCost();
}

QSize
ImageViewer::orgImageSize(int i) const
{
    if (0 <= i && i < countShowImages() && !img_orgs[i].isNull())
    {
        return img_orgs[i].size();
    }
    return QSize();
}

QSize
ImageViewer::combinedImageSize() const
{
    return img_combined.isNull() ? QSize()
                                 : img_combined.size();
}

int
ImageViewer::countShowImages() const
{
    int c = spread_view ? 2 : 1;
    return std::min(c, count());
}

int
ImageViewer::count() const
{
    return img_count;
}

bool
ImageViewer::empty() const
{
    return count() == 0;
}

int
ImageViewer::currentIndex(int i) const
{
    if (0 <= i && i < countShowImages())
    {
        return (index + i) % count();
    }
    return -1;
}

QString
ImageViewer::currentFileName(int i) const
{
    assert(i >= 0);
    const int ti = currentIndex(i);
    if (validIndex(ti))
    {
        return QFileInfo(currentFilePath(i)).fileName();
    }
    return QString();
}

QStringList
ImageViewer::currentFileNames() const
{
    QStringList list;
    const int len = std::min(count(), countShowImages());
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
ImageViewer::currentFilePath(int i) const
{
    assert(i >= 0);
    const int ti = currentIndex(i);
    if (validIndex(ti))
    {
        return playlist->item(ti)->data(Qt::ToolTipRole).toString();
    }
    return QString();
}

QStringList
ImageViewer::currentFilePaths() const
{
    QStringList list;
    const int len = std::min(count(), countShowImages());
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

QDockWidget *
ImageViewer::playlistDock() const
{
    return playlistdock;
}

void
ImageViewer::menu_open_triggered()
{
    QList<QListWidgetItem*> litem = playlist->selectedItems();
    if (litem.empty()) return;

    clearHighlight();
    index = playlist->row(litem.at(0));
    showImages();
    setHighlight();
}

void
ImageViewer::menu_remove_triggered()
{
    QList<QListWidgetItem*> list = playlist->selectedItems();
    if (!list.empty()) playlistItemRemove(list);
}

void
ImageViewer::menu_clear_triggered()
{
    clearPlaylist();
}

void
ImageViewer::playlistItemDoubleClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);
    menu_open_triggered();
}

void
ImageViewer::slideshow_loop()
{
    if (empty())
    {
        stopSlideshow();
    }
    else
    {
        nextImages();
    }
}

void
ImageViewer::prefetcher_prefetchFinished()
{
    for (QList<QListWidgetItem*>::iterator i = prefetch_old.begin();
            i != prefetch_old.end(); ++i)
    {
        (*i)->setIcon(QIcon());
    }
    for (QList<QListWidgetItem*>::iterator i = prefetch_now.begin();
            i != prefetch_now.end(); ++i)
    {
        (*i)->setIcon(prefetched_icon);
    }
}

void
ImageViewer::drag_check()
{
    int d =
        (click_pos.x() - move_pos.x()) * (click_pos.x() - move_pos.x()) +
        (click_pos.y() - move_pos.y()) * (click_pos.y() - move_pos.y());
    if (!is_drag_image && d <= 25) nextImages();
    is_drag_image = false;
    drag_timer.stop();
}

void
ImageViewer::keyPressEvent(QKeyEvent *event)
{
    QGraphicsView::keyPressEvent(event);
    if (event->key() == Qt::Key_Left)  previousImages();
    if (event->key() == Qt::Key_Right) nextImages();
}

void
ImageViewer::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    refresh();
}

void
ImageViewer::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void
ImageViewer::dragLeaveEvent(QDragLeaveEvent *event)
{
    // 何もしてないオーバーライドしているメソッドですが
    // 消すとD&Dがうまくいきません
    Q_UNUSED(event);
}

void
ImageViewer::dragMoveEvent(QDragMoveEvent *event)
{
    // 何もしてないオーバーライドしているメソッドですが
    // 消すとD&Dがうまくいきません
    Q_UNUSED(event);
}

void
ImageViewer::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();
    const QList<QUrl> urls = mime->urls();

    QStringList list;
    for (QList<QUrl>::const_iterator i = urls.constBegin();
            i != urls.constEnd(); ++i)
    {
        const QString path = i->toLocalFile();
        if (QFileInfo(path).exists()) list << path;
    }

    if (!isCopyDrop(event->keyboardModifiers())) clearPlaylist();
    openImages(list);
}

void
ImageViewer::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if (event->buttons() & Qt::RightButton)
    {
        previousImages();
    }
    else
    {
        if (vmode == FULLSIZE || vmode == CUSTOM_SCALE)
        {
            move_pos = click_pos = event->pos();
            drag_timer.start(100);
        }
        else if (event->buttons() & Qt::LeftButton)
        {
            nextImages();
        }
    }
}

void
ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    if (vmode == FULLSIZE || vmode == CUSTOM_SCALE)
    {
        is_drag_image = true;
        move_pos = event->pos();
    }
}

void
ImageViewer::releaseImages()
{
    img_orgs.clear();
    img_combined = QImage();
    img_scaled = QImage();
    img_count = 0;
    index = -1;
    setGraphicsPixmapItem(QImage());
}

void
ImageViewer::showImages()
{
    img_orgs.clear();
    QStringList list = currentFilePaths();
    for (QStringList::const_iterator i = list.constBegin();
            i != list.constEnd(); ++i)
    {
        QImage temp;
        QByteArray *data = readImageData(*i);
        temp.loadFromData(*data);
        free(data);
        img_orgs << (temp.format() == QImage::Format_ARGB32 ? temp
                : temp.convertToFormat(QImage::Format_ARGB32));
    }

    startPrefetch();

    imageCombine(img_orgs);
    imageScale();
    setGraphicsPixmapItem(img_scaled);
    verticalScrollBar()->setSliderPosition(0);
    emit changeImage();
}

void
ImageViewer::refresh()
{
    if (!empty())
    {
        imageScale();
        setGraphicsPixmapItem(img_scaled);
    }
}

void
ImageViewer::setGraphicsPixmapItem(const QImage &img)
{
    view_item->setPixmap(QPixmap::fromImage(img));
    view_scene->setSceneRect(0.0, 0.0, img.width(), img.height());
}

void
ImageViewer::imageScale()
{
    qreal scale = 1.0;

    if (empty()) return;

    if (vmode == ImageViewer::CUSTOM_SCALE)
    {
        scale = scale_value;
    }
    else if (vmode == ImageViewer::FULLSIZE)
    {
        scale = 1.0;
    }
    else
    {
        qreal ws = 1.0, hs = 1.0;
        if (width() < img_combined.width())
        {
            ws = ((qreal)width()) / ((qreal)img_combined.width());
        }

        if (height() < img_combined.height())
        {
            hs = ((qreal)height()) / ((qreal)img_combined.height());
        }

        if (vmode == ImageViewer::FIT_WINDOW)
        {
            scale = ws > hs ? hs : ws;
        }
        else if (vmode == ImageViewer::FIT_IMAGE)
        {
            scale = ws;
        }
    }
    scale_value = scale;

    if (qFuzzyCompare(scale_value, 1.0))
    {
        img_scaled = img_combined;
    }
    else
    {
        img_scaled = (QImage (*[])(const QImage, const qreal)){nn, bl, bc}
                        [imode](img_combined, scale_value);
    }
}

void
ImageViewer::imageCombine(const QVector<QImage> &imgs)
{
    int cw = 0;
    int ch = 0;
    QVector<QImage> vec;
    const int len = imgs.count();
    for (int i = 0; i < len; ++i)
    {
        QImage temp = imgs[i];
        cw += temp.width();
        ch = std::max(ch, temp.height());
        vec << temp;
    }

    img_combined = QImage(cw, ch, QImage::Format_ARGB32);

    if (isRightbindingMode())
    {
        // reverse vector
        for (int i = 0; i < len; ++i) vec.append(vec[len-i-1]);
        vec.remove(0, len);
    }

    int sw = 0;
    QRgb *dst_bits = (QRgb*)img_combined.bits();
    for (QVector<QImage>::const_iterator i = vec.constBegin();
            i != vec.constEnd(); ++i)
    {
        const QImage *temp = i;
        const int tw = temp->width();
        const int th = temp->height();
        const int h2 = (ch - th) / 2;
        const QRgb *src_bits = (QRgb*)temp->constBits();

        for (int y = 0; y < h2; ++y)
        {
            for (int x = 0; x < tw; ++x)
            {
                *(dst_bits+x+sw+y*cw) = qRgba(255, 255, 255, 255);
            }
        }
        for (int y = 0 ; y < th; ++y)
        {
            for (int x = 0; x < tw; ++x)
            {
                *(dst_bits+x+sw+(y+h2)*cw) = *(src_bits+x+y*tw);
            }
        }
        for (int y = th+h2 ; y < ch; ++y)
        {
            for (int x = 0; x < tw; ++x)
            {
                *(dst_bits+x+sw+y*cw) = qRgba(255, 255, 255, 255);
            }
        }
        sw += tw;
    }
    vec.clear();
}

void
ImageViewer::createPlaylistMenus()
{
    playlistdock->setContextMenuPolicy(Qt::ActionsContextMenu);

    menu_open = new QAction(tr("開く"), playlist);
    menu_sep1 = new QAction(playlist);
    menu_sep1->setSeparator(true);
    menu_remove = new QAction(tr("削除する"), playlist);
    menu_clear = new QAction(tr("全て削除する"), playlist);

    playlistdock->addAction(menu_open);
    playlistdock->addAction(menu_sep1);
    playlistdock->addAction(menu_remove);
    playlistdock->addAction(menu_clear);

    connect(menu_open,      SIGNAL(triggered()),
            this, SLOT(menu_open_triggered()));
    connect(menu_remove,    SIGNAL(triggered()),
            this, SLOT(menu_remove_triggered()));
    connect(menu_clear,     SIGNAL(triggered()),
            this, SLOT(menu_clear_triggered()));
}

void
ImageViewer::playlistItemRemove(QList<QListWidgetItem*> items)
{
    if (items.empty()) return;

    bool contains = false;
    QListWidgetItem *current = validIndex(index) ? playlist->item(index)
                                                 : nullptr;
    QList<QListWidgetItem*> cs;
    const int len = std::min(count(), countShowImages());
    for (int i = 0; i < len; ++i)
    {
        const int ti = (index+i)%count();
        cs << playlist->item(ti);
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
            const int r = playlist->row(*i);
            if (r < index) index--;
        }
        playlist->removeItemWidget(*i);
        delete *i;
        img_count--;
    }

    if (empty())
    {
        releaseImages();
        emit changeImage();
    }
    else
    {
        setHighlight();
        if (contains) showImages();
    }
}

void
ImageViewer::openFilesAndDirs(const QStringList &paths, int level)
{
    if (paths.empty()) return;

    for (QStringList::const_iterator i = paths.constBegin();
            i != paths.constEnd(); ++i)
    {
        const QFileInfo info(*i);
        if (info.isFile())
        {
            QListWidgetItem *newitem =
                new QListWidgetItem(info.fileName(), playlist);
            newitem->setData(Qt::ToolTipRole, *i);
            playlist->addItem(newitem);
            img_count++;
        }
        else if (level > 0)
        {
            QStringList newlist;
            const QFileInfoList entrylist = QDir(*i).entryInfoList();
            const int canonical_len = info.canonicalPath().length();
            for (QFileInfoList::const_iterator j = entrylist.constBegin();
                    j != entrylist.constEnd(); ++j)
            {
                if (canonical_len < j->canonicalPath().length())
                {
                    newlist << j->filePath();
                }
            }
            openFilesAndDirs(newlist, level-1);
        }
    }

    if (!validIndex(index) && validIndex(0))
    {
        index = 0;
    }
}

void
ImageViewer::setHighlight()
{
    const int len = std::min(count(), countShowImages());
    for (int i = 0; i < len; ++i)
    {
        playlist->item(currentIndex(i))->setBackground(selectedBC);
    }
}

void
ImageViewer::clearHighlight()
{
    const int len = std::min(count(), countShowImages());
    for (int i = 0; i < len; ++i)
    {
        const int ti = currentIndex(i);
        if (validIndex(ti))
        {
            playlist->item(ti)->setBackground(normalBC);
        }
    }
}

void
ImageViewer::nextImages()
{
    if (empty()) return;

    clearHighlight();
    index = (index + countShowImages()) % count();
    showImages();
    setHighlight();
}

void
ImageViewer::previousImages()
{
    if (empty()) return;

    clearHighlight();
    index = (index - countShowImages()) % count();
    if (index < 0) index += count();
    showImages();
    setHighlight();
}

bool
ImageViewer::validIndex(int i) const
{
     return (0 <= i && i < count());
}

bool
ImageViewer::isCopyDrop(const Qt::KeyboardModifiers km)
{
#ifdef __APPLE__
    return (km & Qt::AltModifier) == Qt::AltModifier;
#else
    return (km & Qt::ControlModifier) == Qt::ControlModifier;
#endif
}

void
ImageViewer::startPrefetch()
{
    if (!prefetcher.isRunning() && !empty())
    {
        prefetch_old = prefetch_now;
        prefetch_now.clear();

        QStringList list;
        int c = count();
        int plen = std::min(getImageCacheSize(), c);
        int l = plen/2;
        int r = plen/2+plen%2;
        for (int i = -l; i < r; i++)
        {
            int ti = (index + i) % c;
            if (ti < 0) ti += c;
            QListWidgetItem *li = playlist->item(ti);
            list << li->data(Qt::ToolTipRole).toString();
            prefetch_now << li;
        }

        prefetcher.setPrefetchImage(list);
        prefetcher.start();
    }
}

QByteArray*
ImageViewer::readImageData(const QString &path)
{
    prefetch_mutex.lock();
    bool c = cache.contains(path);
    prefetch_mutex.unlock();
    if (c)
    {
        prefetch_mutex.lock();
        QByteArray *data = cache[path];
        QByteArray *ret = new QByteArray(*data);
        prefetch_mutex.unlock();
        return ret;
    }
    else
    {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly))
        {
            QByteArray *data = new QByteArray(file.readAll());
            QByteArray *ret = new QByteArray(*data);
            prefetch_mutex.lock();
            if (cache.maxCost() > 0) cache.insert(path, data);
            prefetch_mutex.unlock();
            return ret;
        }
        else
        {
            return nullptr;
        }
    }
}

Prefetcher::Prefetcher(QCache<QString, QByteArray> *ch, QMutex *m)
    : QThread()
    , cache(ch)
    , mutex(m)
{
}

Prefetcher::~Prefetcher()
{
}

void
Prefetcher::setPrefetchImage(QStringList &list)
{
    plist = QStringList(list);
}

void
Prefetcher::run()
{
    for (QStringList::const_iterator i = plist.constBegin();
            i != plist.constEnd(); ++i)
    {
        QString path = *i;
        mutex->lock();
        bool c = cache->contains(path);
        mutex->unlock();

        if (!c)
        {
            QFile file(path);
            if (file.open(QIODevice::ReadOnly))
            {
                QByteArray *data = new QByteArray(file.readAll());
                mutex->lock();
                if (cache->maxCost() > 0)
                {
                    cache->insert(path, data, 1);
                }
                else
                {
                    mutex->unlock();
                    emit prefetchFinished();
                    return;
                }
                mutex->unlock();
            }
        }
    }
    emit prefetchFinished();
}

