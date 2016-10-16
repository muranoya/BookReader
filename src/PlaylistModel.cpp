#include "PlaylistModel.hpp"
#include <QFileInfo>
#include <QDir>

#include "for_windows_env.hpp"

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel(parent)
    , slct(new QItemSelectionModel(this))
    , files()
    , spreadview(false)
    , rbindview(false)
    , opendirlevel(1)
    , img_index(-1)
    , prft(new Prefetcher())
{
}

PlaylistModel::~PlaylistModel()
{
    qDeleteAll(files);
    files.clear();
}

QVariant
PlaylistModel::data(const QModelIndex &idx, int role) const
{
    int row = idx.row();
    if (!idx.isValid() || !isValidIndex(row) || !files[row])
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::DisplayRole:
            return files[row]->logicalFileName();
        case Qt::ToolTipRole:
            return files[row]->physicalFilePath();
        case Qt::BackgroundRole:
            if (isCurrentIndex(row))
            {
                return QBrush(Qt::lightGray);
            }
            break;
    }

    return QVariant();
}

int
PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return files.count();
}

void
PlaylistModel::setSpreadView(bool spread)
{
    bool c = (spreadview != spread);
    spreadview = spread;
    if (c)
    {
        emit changeImage(combineImage());
        emit changePlaylistStatus();
        dataChangeNotice(img_index);
    }
}

bool
PlaylistModel::getSpreadView() const
{
    return spreadview;
}

void
PlaylistModel::setRightbindingView(bool rb)
{
    bool c = (rbindview != rb);
    rbindview = rb;
    if (c)
    {
        emit changeImage(combineImage());
        emit changePlaylistStatus();
        dataChangeNotice(img_index);
    }
}

bool
PlaylistModel::getRightbindingView() const
{
    return rbindview;
}

void
PlaylistModel::setOpenDirLevel(int n)
{
    opendirlevel = n;
}

int
PlaylistModel::getOpenDirLevel() const
{
    return opendirlevel;
}

void
PlaylistModel::setCacheSize(int n)
{
    prft->setCacheSize(n);
}

int
PlaylistModel::getCacheSize() const
{
    return prft->getCacheSize();
}

int
PlaylistModel::countShowImages() const
{
    int c = (getSpreadView() ? 2 : 1);
    return std::min(count(), c);
}

int
PlaylistModel::count() const
{
    return files.count();
}

bool
PlaylistModel::empty() const
{
    return count() == 0;
}

int
PlaylistModel::currentIndex(int i) const
{
    if (0 <= i && i < countShowImages())
    {
        return (img_index + i) % count();
    }
    return -1;
}

QString
PlaylistModel::currentFileName(int i) const
{
    const int ti = currentIndex(i);
    if (isValidIndex(ti))
    {
        return files[ti]->logicalFileName();
    }
    return QString();
}

void
PlaylistModel::setModelToItemView(QAbstractItemView *view)
{
    if (!view) return;

    view->setModel(this);
    view->setSelectionModel(slct);
    connect(view, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(itemViewDoubleClicked(const QModelIndex &)));
}

void
PlaylistModel::openImages(const QStringList &path)
{
    bool req_refresh = (countShowImages() == 0 ||
        (getRightbindingView() && countShowImages() != 2));

    int c = openFilesAndDirs(path, getOpenDirLevel());

    if (req_refresh && !empty())
    {
        dataChangeNotice(0);
        emit changeImage(combineImage());
    }
    if (c > 0)
    {
        emit changePlaylistStatus();
    }
}

void
PlaylistModel::showSelectedItem()
{
    QModelIndexList list = slct->selectedRows();
    if (list.empty()) return;

    dataChangeNotice(list[0].row());
    emit changeImage(combineImage());
    emit changePlaylistStatus();
}

void
PlaylistModel::removeSelectedItem()
{
    QModelIndexList list = slct->selectedRows();
    if (list.empty()) return;

    int c = 0;
    for (QModelIndexList::const_iterator iter = list.constBegin();
            iter != list.constEnd(); ++iter)
    {
        int row = (*iter).row();
        if (row < img_index) c++;
        delete files[row];
        files[row] = nullptr;
    }

    bool contain = (files[currentIndex(0)] == nullptr) ||
        (countShowImages() > 1 && (files[currentIndex(1)] == nullptr));

    for (int i = 0; i < files.count();)
    {
        if (files[i] == nullptr)
        {
            beginRemoveRows(QModelIndex(), i, i);
            files.remove(i);
            endRemoveRows();
        }
        else
        {
            i++;
        }
    }

    if (empty())
    {
        dataChangeNotice(-1);
        emit changeImage(QImage());
        emit changePlaylistStatus();
    }
    else
    {
        dataChangeNotice(img_index-c);
        if (contain)
        {
            emit changeImage(combineImage());
            emit changePlaylistStatus();
        }
    }
}

void
PlaylistModel::clearPlaylist()
{
    if (empty()) return;

    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
    {
        qDeleteAll(files);
        files.clear();
    }
    endRemoveRows();

    img_index = -1;
    emit changeImage(QImage());
    emit changePlaylistStatus();
}

void
PlaylistModel::nextImage()
{
    if (empty()) return;

    int old_index = img_index;

    dataChangeNotice(nextIndex(img_index, countShowImages()));
    
    if (old_index != img_index)
    {
        emit changeImage(combineImage());
        emit changePlaylistStatus();
    }
}

void
PlaylistModel::prevImage()
{
    if (empty()) return;

    int old_index = img_index;

    dataChangeNotice(nextIndex(img_index, -countShowImages()));

    if (old_index != img_index)
    {
        emit changeImage(combineImage());
        emit changePlaylistStatus();
    }
}

void
PlaylistModel::itemViewDoubleClicked(const QModelIndex &img_index)
{
    Q_UNUSED(img_index);
    showSelectedItem();
}

int
PlaylistModel::nextIndex(int idx, int c) const
{
    int i = (idx + c) % count();
    if (i < 0) i += count();
    return i;
}

bool
PlaylistModel::isValidIndex(int i) const
{
    return (0 <= i && i < count());
}

bool
PlaylistModel::isCurrentIndex(int idx) const
{
    for (int n = 0; n < countShowImages(); ++n)
    {
        if (idx == currentIndex(n)) return true;
    }
    return false;
}

void
PlaylistModel::dataChangeNotice(int newidx)
{
    int len = countShowImages();
    for (int i = 0; i < len; ++i)
    {
        int idx = currentIndex(i);
        if (isValidIndex(idx))
        {
            QModelIndex midx = index(idx, 0);
            dataChanged(midx, midx);
        }
    }

    img_index = newidx;

    if (newidx >= 0)
    {
        for (int i = 0; i < len; ++i)
        {
            int idx = currentIndex(i);
            if (isValidIndex(idx))
            {
                QModelIndex midx = index(idx, 0);
                dataChanged(midx, midx);
            }
        }
    }

    {
        QVector<ImageFile> list;
        int num = std::min(files.count(), prft->getCacheSize());
        for (int i = 1, n = 0; n < num; ++i)
        {
            list.append(*files.at(nextIndex(img_index, i)));  n++;
            if (n+1 >= num) break;
            list.append(*files.at(nextIndex(img_index, -i))); n++;
        }
        prft->putRequest(list);
        list.clear();
    }
}

int
PlaylistModel::openFilesAndDirs(const QStringList &paths, int level)
{
    QVector<ImageFile*> openfiles;
    openFilesAndDirs0(openfiles, paths, level);
    if (!openfiles.empty())
    {
        int c = openfiles.count();
        beginInsertRows(QModelIndex(), rowCount(),
                rowCount() + c - 1);
        files.append(openfiles);
        endInsertRows();
        openfiles.clear();
        return c;
    }
    return 0;
}

void
PlaylistModel::openFilesAndDirs0(QVector<ImageFile*> &openfiles,
        const QStringList &paths, int level)
{
    if (paths.empty()) return;

    for (QStringList::const_iterator iter = paths.constBegin();
            iter != paths.constEnd(); ++iter)
    {
        const QFileInfo info(*iter);
        if (info.isFile())
        {
            if (ImageFile::isReadableImageFile(*iter))
            {
                ImageFile *imgfile = new ImageFile(*iter);
                openfiles.append(imgfile);
            }
            else
            {
                QVector<ImageFile*> imgfiles =
                    ImageFile::openArchive(*iter);
                openfiles.append(imgfiles);
            }
        }
        else if (level > 0)
        {
            QStringList newlist;
            const QFileInfoList entrylist = QDir(*iter).entryInfoList();
            const int clen = info.canonicalPath().length();
            for (QFileInfoList::const_iterator iter2 =
                    entrylist.constBegin();
                    iter2 != entrylist.constEnd(); ++iter2)
            {
                if (clen < iter2->canonicalPath().length())
                {
                    newlist << iter2->filePath();
                }
            }
            openFilesAndDirs0(openfiles, newlist, level-1);
        }
    }
}

QImage
PlaylistModel::loadData(const ImageFile &f)
{
    QImage img;
    QByteArray *data = prft->get(f.createKey());
    if (data)
    {
        fprintf(stderr, "cache hit\n");
        img.loadFromData(*data);
    }
    else
    {
        fprintf(stderr, "cache miss\n");
        data = f.readData();
        if (!data) return QImage();
        img.loadFromData(*data);
        delete data;
    }

    if (img.format() != QImage::Format_ARGB32)
    {
        return img.convertToFormat(QImage::Format_ARGB32);
    }
    return img;
}

QImage
PlaylistModel::combineImage()
{
    QImage imgs[2];
    switch (countShowImages())
    {
        case 1:
            return loadData(*files[currentIndex(0)]);
        case 2:
            if (getRightbindingView())
            {
                imgs[0] = loadData(*files[currentIndex(1)]);
                imgs[1] = loadData(*files[currentIndex(0)]);
            }
            else
            {
                imgs[0] = loadData(*files[currentIndex(0)]);
                imgs[1] = loadData(*files[currentIndex(1)]);
            }
            break;
        default:
            return QImage();
    }

    int sw = 0;
    int cw = imgs[0].width() + imgs[1].width();
    int ch = std::max(imgs[0].height(), imgs[1].height());
    QImage cimg(cw, ch, QImage::Format_ARGB32);
    QRgb *dst = (QRgb*)cimg.bits();
    for (int i = 0; i < 2; ++i)
    {
        const QImage *temp = imgs+i;
        const int tw = temp->width();
        const int th = temp->height();
        const int h2 = (ch - th) / 2;
        const QRgb *src = (QRgb*)temp->constBits();

        for (int y = 0; y < h2; ++y)
        {
            for (int x = 0; x < tw; ++x)
            {
                *(dst+x+sw+y*cw) = qRgba(0, 0, 0, 0);
            }
        }
        for (int y = 0 ; y < th; ++y)
        {
            for (int x = 0; x < tw; ++x)
            {
                *(dst+x+sw+(y+h2)*cw) = *(src+x+y*tw);
            }
        }
        for (int y = th+h2 ; y < ch; ++y)
        {
            for (int x = 0; x < tw; ++x)
            {
                *(dst+x+sw+y*cw) = qRgba(0, 0, 0, 0);
            }
        }
        sw += tw;
    }
    return cimg;
}

