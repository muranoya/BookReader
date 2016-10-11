#include "ImageViewer.hpp"
#include "TextEncDialog.hpp"
#include "image.hpp"

#include <algorithm>
#include <cassert>

#include <QMimeData>
#include <QDir>
#include <QPainter>

#include <archive.h>
#include <archive_entry.h>

static const QString readable_suffix[] = 
{
    "jpg", "jpeg",
    "png",
    "pbm", "ppm", "pgm", "pnm",
    "bmp",
    "gif",
    "tiff",
    "xbm",
    "xpm",
};
static const QString readable_format(
"Images ("
"*.jpg *.jpeg "
"*.png "
"*.pbm *.ppm *.pgm *.pnm "
"*.bmp "
"*.gif "
"*.tiff "
"*.xbm "
"*.xpm"
")\n"
"Archives ("
"*.zip "
"*.tar "
"*.7z "
"*.tar "
"*.cab "
"*.gz "
"*.bz2"
")\n"
"All Files (*.*)");

ImageViewer::ImageViewer(QWidget *parent)
    : QWidget(parent)
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
    , img_pos()
    /* playlist */
    , playlistdock(new QDockWidget(tr("プレイリスト"), parent))
    , playlist(new QListWidget())
    , menu_open(nullptr)
    , menu_sep1(nullptr)
    , menu_remove(nullptr)
    , menu_clear(nullptr)
    , menu_sep2(nullptr)
    , menu_enc(nullptr)
    , normalBC(Qt::transparent)
    , selectedBC(Qt::lightGray)
    , index(-1)
    , spread_view(false)
    , opendirlevel(30)
    /* prefetch */
    , cache(20)
    , prfter(&cache, &prft_mutex)
    , prft_mutex()
    , prft_old()
    , prft_now()
    , prft_icon(":/check.png")
{
    playlistdock->setWidget(playlist);

    createPlaylistMenus();

    connect(playlist,    SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(playlistItemDoubleClicked(QListWidgetItem*)));
    
    connect(playlist,    SIGNAL(itemSelectionChanged()),
            this, SLOT(playlistItemSelectionChanged()));
    
    connect(&drag_timer, SIGNAL(timeout()),
            this, SLOT(drag_check()));

    connect(&prfter,     SIGNAL(finished()),
            this, SLOT(prefetcherFinished()),
            Qt::QueuedConnection);

    playlist->setDefaultDropAction(Qt::MoveAction);
    playlist->setSelectionMode(QAbstractItemView::ExtendedSelection);

    setFocusPolicy(Qt::StrongFocus);
    setAcceptDrops(true);
}

ImageViewer::~ImageViewer()
{
    delete menu_open;
    delete menu_sep1;
    delete menu_remove;
    delete menu_clear;
    delete menu_sep2;
    delete menu_enc;
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

    prft_old.clear();
    prft_now.clear();
    prfter.sendTermSig();

    while (playlist->count() > 0)
    {
        delete playlist->takeItem(0);
    }
    releaseImages();
    if (c) emit changedImage();
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
ImageViewer::setFeedPageMode(FeedPageMode m)
{
    fpmode = m;
}

ImageViewer::FeedPageMode
ImageViewer::getFeedPageMode() const
{
    return fpmode;
}

bool
ImageViewer::setScale(ViewMode m, double s)
{
    bool c = (m != vmode || s != scale_value);
    if (0.01 <= s && s <= 10)
    {
        vmode = m;
        scale_value = s;
        if (c) refresh();
        return true;
    }
    return false;
}

bool
ImageViewer::setScale(ViewMode m)
{
    return setScale(m, scale_value);
}

double
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
    prft_mutex.lock();
    cache.setMaxCost(cost);
    prft_mutex.unlock();
    
    startPrefetch();
}

int
ImageViewer::getImageCacheSize() const
{
    return cache.maxCost();
}

QSize
ImageViewer::imageSize() const
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

QStringList
ImageViewer::currentFileNames() const
{
    QStringList list;
    const int len = std::min(count(), countShowImages());
    for (int i = 0; i < len; ++i)
    {
        list << currentFileName(i);
    }
    return list;
}

QString
ImageViewer::currentFileName(int i) const
{
    assert(i >= 0);
    const int ti = currentIndex(i);
    if (validIndex(ti))
    {
        PlayListItem *item = static_cast<PlayListItem*>(playlist->item(ti));
        return item->file().logicalFileName();
    }
    return QString();
}

QDockWidget *
ImageViewer::playlistDock() const
{
    return playlistdock;
}

const QString&
ImageViewer::readableExtFormat() const
{
    return readable_format;
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
ImageViewer::menu_enc_triggered()
{
    QList<QListWidgetItem*> items = playlist->selectedItems();
    if (items.empty()) return;

    PlayListItem *x = static_cast<PlayListItem*>(items.first());
    if (x->file().fileType() != ImageViewer::File::ARCHIVE) return;

    QTextCodec *c = TextEncDialog::selectTextCodec(x->file().rawFilePath());
    if (c == nullptr) return;

    QString apath = x->file().physicalFilePath();
    int len = playlist->count();
    for (int i = 0; i < len; ++i)
    {
        PlayListItem *item = static_cast<PlayListItem*>(playlist->item(i));
        if (apath.compare(item->file().physicalFilePath()) == 0)
        {
            item->file().changeTextEnc(c);
            item->refreshText();
        }
    }
}

void
ImageViewer::playlistItemDoubleClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);
    menu_open_triggered();
}

void
ImageViewer::playlistItemSelectionChanged()
{
    QList<QListWidgetItem*> items = playlist->selectedItems();

    menu_open->setEnabled(false);
    menu_remove->setEnabled(false);
    menu_enc->setEnabled(false);

    if (items.empty()) return;

    menu_open->setEnabled(true);
    menu_remove->setEnabled(true);

    for (QList<QListWidgetItem*>::const_iterator i = items.constBegin();
            i != items.constEnd(); ++i)
    {
        PlayListItem *pitem = static_cast<PlayListItem*>(*i);
        if (pitem->file().fileType() == ImageViewer::File::ARCHIVE)
        {
            menu_enc->setEnabled(true);
            return;
        }
    }
}

void
ImageViewer::prefetcherFinished()
{
    for (QList<QListWidgetItem*>::iterator i = prft_old.begin();
            i != prft_old.end(); ++i)
    {
        (*i)->setIcon(QIcon());
    }
    for (QList<QListWidgetItem*>::iterator i = prft_now.begin();
            i != prft_now.end(); ++i)
    {
        (*i)->setIcon(prft_icon);
    }
}

void
ImageViewer::drag_check()
{
    is_drag_image = true;
    drag_timer.stop();
}

void
ImageViewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::CrossPattern);

    if (img_scaled.isNull()) return;

    switch (getScaleMode())
    {
        case ImageViewer::FIT_WINDOW:
        {
            int x = std::max(width() - img_scaled.width(), 0);
            int y = std::max(height() - img_scaled.height(), 0);
            painter.drawImage(QPoint(x/2, y/2), img_scaled);
            break;
        }
        case ImageViewer::FIT_IMAGE:
        {
            int x = std::max(width() - img_scaled.width(), 0);
            img_pos += move_pos - click_pos2;
            click_pos2 = move_pos;
            painter.drawImage(QPoint(x/2, img_pos.y()), img_scaled);
            break;
        }
        default:
            img_pos += move_pos - click_pos2;
            click_pos2 = move_pos;
            painter.drawImage(img_pos, img_scaled);
    }
}

void
ImageViewer::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
    if (event->key() == Qt::Key_Left)  previousImages();
    if (event->key() == Qt::Key_Right) nextImages();
}

void
ImageViewer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    refresh();
}

void
ImageViewer::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
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
    is_drag_image = false;
    if (getFeedPageMode() == ImageViewer::MouseButton)
    {
        if (event->buttons() & Qt::RightButton)
        {
            previousImages();
        }
        else if (event->buttons() & Qt::LeftButton)
        {
            if (getScaleMode() != FIT_WINDOW)
            {
                move_pos = click_pos = click_pos2 = event->pos();
                drag_timer.start(120);
            }
            else
            {
                nextImages();
            }
        }
    }
    else
    {
        if (event->buttons() & Qt::LeftButton)
        {
            if (getScaleMode() != FIT_WINDOW)
            {
                move_pos = click_pos = click_pos2 = event->pos();
                drag_timer.start(120);
            }
            else
            {
                int th = width() / 2;
                if (event->pos().x() < th)
                {
                    previousImages();
                }
                else
                {
                    nextImages();
                }
            }
        }
    }
    event->accept();
}

void
ImageViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (getFeedPageMode() == ImageViewer::MouseButton)
    {
        if (getScaleMode() != FIT_WINDOW)
        {
            if (event->button() == Qt::LeftButton && !is_drag_image)
            {
                nextImages();
            }
        }
    }
    else
    {
        if (getScaleMode() != FIT_WINDOW)
        {
            if (event->button() == Qt::LeftButton && !is_drag_image)
            {
                int th = width() / 2;
                if (event->pos().x() < th)
                {
                    previousImages();
                }
                else
                {
                    nextImages();
                }
            }
        }
    }
    event->accept();
}

void
ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (getScaleMode() != FIT_WINDOW)
    {
        move_pos = event->pos();
        update();
    }
    event->accept();
}

void
ImageViewer::wheelEvent(QWheelEvent *event)
{
    if (empty()) return;

    if (event->modifiers() & Qt::ShiftModifier)
    {
        bool sign= event->delta() < 0;
        double v = 0.05;
        if (sign) v *= -1.0;
        v += getScale();
        if (setScale(CUSTOM_SCALE, v))
        {
            emit changedScaleMode();
        }
    }
    else
    {
        if (getScaleMode() != FIT_WINDOW)
        {
            int steps = event->delta() / 4;
            if (event->modifiers() & Qt::ControlModifier)
            {
                img_pos.rx() += steps;
            }
            else
            {
                img_pos.ry() += steps;
            }
            update();
        }
    }
    event->accept();
}

bool
ImageViewer::isReadableImageFile(const QString &path) const
{
    QString suf = QFileInfo(path).suffix().toLower();
    int len = (int)(sizeof(readable_suffix)/sizeof(readable_suffix[0]));
    for (int i = 0; i < len; ++i)
    {
        if (suf.compare(readable_suffix[i]) == 0) return true;
    }
    return false;
}

void
ImageViewer::releaseImages()
{
    img_combined = QImage();
    img_scaled = QImage();
    img_count = 0;
    index = -1;
    update();
}

void
ImageViewer::showImages()
{
    QList<File> list = currentFiles();
    QVector<QImage> imgs;
    for (QList<File>::const_iterator i = list.constBegin();
            i != list.constEnd(); ++i)
    {
        QImage temp;
        QByteArray *data = readData(*i);
        temp.loadFromData(*data);
        delete data;
        imgs << ((temp.format() == QImage::Format_ARGB32)
                ? temp
                : temp.convertToFormat(QImage::Format_ARGB32));
    }

    startPrefetch();

    imageCombine(imgs);
    imageScale();

    int x = width() - img_scaled.width();
    int y = height() - img_scaled.height();
    x = (x < 0 ? 0 : x/2);
    y = (y < 0 ? 0 : y/2);
    img_pos = QPoint(x, y);
    update();

    emit changedImage();
    imgs.clear();
}

void
ImageViewer::refresh()
{
    if (!empty())
    {
        imageScale();
    }
}

void
ImageViewer::imageScale()
{
    double scale = 1.0;

    if (empty()) return;

    if (vmode == ImageViewer::CUSTOM_SCALE)  scale = scale_value;
    else if (vmode == ImageViewer::FULLSIZE) scale = 1.0;
    else
    {
        double ws = 1.0, hs = 1.0;
        if (width() < img_combined.width())
        {
            ws = static_cast<double>(width()) / static_cast<double>(img_combined.width());
        }

        if (height() < img_combined.height())
        {
            hs = static_cast<double>(height()) / static_cast<double>(img_combined.height());
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
        QImage (*f[])(const QImage &, const double) = {nn, bl, bc};
        img_scaled = f[imode](img_combined, scale_value);
    }
    update();
}

void
ImageViewer::imageCombine(const QVector<QImage> &imgs)
{
    int cw = 0, ch = 0;
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
                *(dst_bits+x+sw+y*cw) = qRgba(0, 0, 0, 0);
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
                *(dst_bits+x+sw+y*cw) = qRgba(0, 0, 0, 0);
            }
        }
        sw += tw;
    }
    vec.clear();
}

bool
ImageViewer::validIndex(int i) const
{
     return (0 <= i && i < count());
}

void
ImageViewer::createPlaylistMenus()
{
    playlistdock->setContextMenuPolicy(Qt::ActionsContextMenu);

    menu_open   = new QAction(tr("開く"), playlist);
    menu_open->setEnabled(false);
    menu_sep1   = new QAction(playlist);
    menu_sep1->setSeparator(true);
    menu_remove = new QAction(tr("削除する"), playlist);
    menu_remove->setEnabled(false);
    menu_clear  = new QAction(tr("全て削除する"), playlist);
    menu_clear->setEnabled(true);
    menu_sep2   = new QAction(playlist);
    menu_sep2->setSeparator(true);
    menu_enc    = new QAction(tr("文字コードの設定"), playlist);
    menu_enc->setEnabled(false);

    playlistdock->addAction(menu_open);
    playlistdock->addAction(menu_sep1);
    playlistdock->addAction(menu_remove);
    playlistdock->addAction(menu_clear);
    playlistdock->addAction(menu_sep2);
    playlistdock->addAction(menu_enc);

    connect(menu_open,      SIGNAL(triggered()),
            this, SLOT(menu_open_triggered()));

    connect(menu_remove,    SIGNAL(triggered()),
            this, SLOT(menu_remove_triggered()));
   
    connect(menu_clear,     SIGNAL(triggered()),
            this, SLOT(menu_clear_triggered()));
    
    connect(menu_enc,       SIGNAL(triggered()),
            this, SLOT(menu_enc_triggered()));
}

void
ImageViewer::playlistItemRemove(const QList<QListWidgetItem*> &items)
{
    if (items.empty()) return;

    bool contains = false;
    QListWidgetItem *current = validIndex(index) ? playlist->item(index)
                                              : nullptr;
    QList<QListWidgetItem*> cs;
    const int len = std::min(count(), countShowImages());
    for (int i = 0; i < len; ++i)
    {
        cs << playlist->item((index+i) % count());
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
            if (playlist->row(*i) < index) index--;
        }
        
        prft_old.removeOne(*i); 
        prft_now.removeOne(*i); 

        playlist->removeItemWidget(*i);
        delete *i;
        img_count--;
    }

    if (empty())
    {
        releaseImages();
        emit changedImage();
    }
    else
    {
        setHighlight();
        if (contains) showImages();
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

ImageViewer::File
ImageViewer::currentFile(int i) const
{
    assert(i >= 0);
    const int ti = currentIndex(i);
    if (validIndex(ti))
    {
        PlayListItem *item = static_cast<PlayListItem*>(playlist->item(ti));
        return item->file();
    }
    return ImageViewer::File();
}

QList<ImageViewer::File>
ImageViewer::currentFiles() const
{
    QList<ImageViewer::File> list;
    const int len = std::min(count(), countShowImages());
    for (int i = 0; i < len; ++i)
    {
        list << currentFile(i);
    }
    return list;
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
ImageViewer::openArchiveFile(const QString &path)
{
    struct archive *a;
    struct archive_entry *ae;
    int r;

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    r = archive_read_open_filename(a,
            path.toLocal8Bit().constData(), 1024*128);
    if (r != ARCHIVE_OK)
    {
        fprintf(stderr, "%s\n", archive_error_string(a));
        archive_read_free(a);
        return false;
    }
    while (archive_read_next_header(a, &ae) == ARCHIVE_OK)
    {
        QByteArray raw_entry(archive_entry_pathname(ae));
        QString entry_name(raw_entry);
        if (isReadableImageFile(entry_name))
        {
            archive_read_data_skip(a);
            PlayListItem *newitem = new PlayListItem(path, raw_entry, playlist);
            playlist->addItem(newitem);
            img_count++;
        }
    }
    r = archive_read_free(a);
    if (r != ARCHIVE_OK)
    {
        fprintf(stderr, "%s\n", archive_error_string(a));
        return false;
    }
    return true;
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
            if (isReadableImageFile(*i))
            {
                PlayListItem *newitem = new PlayListItem(*i, playlist);
                playlist->addItem(newitem);
                img_count++;
            }
            else
            {
                openArchiveFile(*i);
            }
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

QByteArray*
ImageViewer::readImageData(const File &f)
{
    QFile file(f.physicalFilePath());
    return file.open(QIODevice::ReadOnly)
        ? new QByteArray(file.readAll())
        : nullptr;
}

QByteArray*
ImageViewer::readArchiveData(const File &f)
{
    struct archive *a;
    struct archive_entry *ae;
    int r;

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    r = archive_read_open_filename(a,
            f.physicalFilePath().toLocal8Bit().constData(), 1024*128);
    if (r != ARCHIVE_OK)
    {
        fprintf(stderr, "%s\n", archive_error_string(a));
        archive_read_free(a);
        return nullptr;
    }

    const QByteArray &s_entry = f.rawFilePath();
    while (archive_read_next_header(a, &ae) == ARCHIVE_OK)
    {
        QByteArray entry(archive_entry_pathname(ae));
        if (s_entry == entry)
        {
            QByteArray *data = new QByteArray();
            const void *buf;
            size_t len;
            off_t offset;

            while (archive_read_data_block(a, &buf, &len, &offset) == ARCHIVE_OK)
            {
                data->append((const char *)buf, len);
            }
            archive_read_free(a);
            if (data->isEmpty())
            {
                delete data;
                return nullptr;
            }
            else
            {
                return data;
            }
        }
        else
        {
            archive_read_data_skip(a);
        }
    }
    archive_read_free(a);
    return nullptr;
}

QByteArray*
ImageViewer::readData(const File &f)
{
    QString key = f.createKey();

    prft_mutex.lock();
    bool c = cache.contains(key);
    prft_mutex.unlock();

    if (c)
    {
        prft_mutex.lock();
        QByteArray *ret = new QByteArray(*cache[key]);
        prft_mutex.unlock();
        return ret;
    }

    QByteArray *data;
    switch (f.fileType())
    {
        case File::ARCHIVE:
            data = readArchiveData(f);
            break;
        case File::RAW:
            data = readImageData(f);
            break;
        default:
            return nullptr;
    }
    if (data == nullptr) return nullptr;

    if (cache.maxCost() > 0)
    {
        QByteArray *ret = new QByteArray(*data);
        prft_mutex.lock();
        cache.insert(key, data);
        prft_mutex.unlock();
        return ret;
    }
    else
    {
        return data;
    }
}

void
ImageViewer::startPrefetch()
{
    if (!prfter.isRunning() && !empty())
    {
        prft_old = prft_now;
        prft_now.clear();

        QList<File> list;
        int c = count();
        int plen = std::min(getImageCacheSize(), c);
        int l = plen/2-1;
        int r = plen/2+plen%2+1;
        for (int i = -l; i < r; i++)
        {
            int ti = (index + i) % c;
            if (ti < 0) ti += c;
            QListWidgetItem *li = playlist->item(ti);
            list << static_cast<PlayListItem*>(li)->file();
            prft_now << li;
        }

        prfter.setPrefetchImage(list);
        prfter.start();
    }
}

bool
ImageViewer::isCopyDrop(const Qt::KeyboardModifiers km) const
{
#ifdef __APPLE__
    return (km & Qt::AltModifier) == Qt::AltModifier;
#else
    return (km & Qt::ControlModifier) == Qt::ControlModifier;
#endif
}

ImageViewer::File::File(const QString &archivePath, const QByteArray &rfilepath)
    : ft(ARCHIVE)
    , archive_path(archivePath)
    , file_path(rfilepath)
    , raw_file_entry(rfilepath)
{
}

ImageViewer::File::File(const QString &filePath)
    : ft(RAW)
    , archive_path()
    , file_path(filePath)
    , raw_file_entry()
{
}

ImageViewer::File::File()
    : ft(INVALID)
    , archive_path()
    , file_path()
    , raw_file_entry()
{
}

ImageViewer::File::~File()
{
    raw_file_entry.clear();
}

ImageViewer::File::FileType
ImageViewer::File::fileType() const
{
    return ft;
}

QString
ImageViewer::File::physicalFilePath() const
{
    assert(ft != INVALID);
    switch (ft)
    {
        case ARCHIVE: return archive_path;
        case RAW:     return file_path;
        default:      return QString();
    }
}

QString
ImageViewer::File::physicalFileName() const
{
    return QFileInfo(physicalFilePath()).fileName();
}

QString
ImageViewer::File::logicalFilePath() const
{
    assert(ft != INVALID);
    switch (ft)
    {
        case ARCHIVE: return file_path;
        case RAW:     return file_path;
        default:      return QString();
    }
}

QString
ImageViewer::File::logicalFileName() const
{
    return QFileInfo(logicalFilePath()).fileName();
}

void
ImageViewer::File::changeTextEnc(const QTextCodec *c)
{
    assert(ft == ARCHIVE && c != nullptr);

    file_path = c->toUnicode(raw_file_entry);
}

const QByteArray&
ImageViewer::File::rawFilePath() const
{
    assert(ft == ARCHIVE);
    return raw_file_entry;
}

QString
ImageViewer::File::createKey() const
{
    assert(ft != INVALID);
    if (ft == ARCHIVE)
    {
        QString ret;
        ret.append(archive_path)
            .append("/")
            .append(QString(file_path));
        return ret;
    }
    else if (ft == RAW)
    {
        return file_path;
    }
    else
    {
        return QString();
    }
}

ImageViewer::PlayListItem::PlayListItem(const QString &p, const QByteArray &f,
        QListWidget *parent)
    : QListWidgetItem(parent)
    , f(p, f)
{
    refreshText();
}

ImageViewer::PlayListItem::PlayListItem(const QString &f, QListWidget *parent)
    : QListWidgetItem(parent)
    , f(f)
{
    refreshText();
}

ImageViewer::PlayListItem::~PlayListItem()
{
}

ImageViewer::File&
ImageViewer::PlayListItem::file()
{
    return f;
}

void
ImageViewer::PlayListItem::refreshText()
{
    QString str;
    if (f.fileType() == ImageViewer::File::ARCHIVE)
    {
        str.append(f.physicalFilePath())
            .append("/")
            .append(f.logicalFileName());
    }
    else
    {
        str.append(f.physicalFilePath());
    }
    setData(Qt::ToolTipRole, str);
    setText(f.logicalFileName());
}

ImageViewer::Prefetcher::Prefetcher(QCache<QString, QByteArray> *ch, QMutex *m)
    : QThread()
    , cache(ch)
    , mutex(m)
    , termsig(false)
{
}

void
ImageViewer::Prefetcher::setPrefetchImage(const QList<File> &list)
{
    plist = QList<File>(list);
}

void
ImageViewer::Prefetcher::sendTermSig()
{
    termsig = true;
}

void
ImageViewer::Prefetcher::run()
{
    termsig = false;
    if (cache->maxCost() == 0) return;
    
    for (QList<File>::const_iterator i = plist.constBegin();
            i != plist.constEnd(); ++i)
    {
        if (termsig) return;

        const File f = *i;
        QString key = f.createKey();

        mutex->lock();
        bool c = cache->contains(key);
        mutex->unlock();

        if (!c)
        {
            QByteArray *data;
            if (f.fileType() == ImageViewer::File::ARCHIVE)
            {
                data = ImageViewer::readArchiveData(f);
            }
            else
            {
                data = ImageViewer::readImageData(f);
            }
            if (data != nullptr)
            {
                mutex->lock();
                cache->insert(key, data, 1);
                mutex->unlock();
            }
        }
    }
}

