#include <QFileInfo>
#include <archive.h>
#include <archive_entry.h>
#include <utility>
#include "ImageFile.hpp"

static const QString readable_image_suffix[] = 
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

static const QString readable_archive_suffix[] =
{
    "zip", "tar", "7z", "cab",
};

ImageFile::ImageFile()
    : ft(INVALID)
    , archive_path()
    , file_path()
    , raw_file_entry()
{
}

ImageFile::ImageFile(const QString &path,
        const QByteArray &entry)
    : ft(ARCHIVE)
    , archive_path(path)
    , file_path(path + "/" + QString(entry))
    , raw_file_entry(entry)
{
}

ImageFile::ImageFile(const QString &imagefile)
    : ft(RAW)
    , archive_path()
    , file_path(imagefile)
    , raw_file_entry()
{
}

ImageFile::ImageFile(const ImageFile &other)
    : ft(other.ft)
    , archive_path(other.archive_path)
    , file_path(other.file_path)
    , raw_file_entry(other.raw_file_entry)
{
}

ImageFile::ImageFile(ImageFile &&other)
    : ft(other.ft)
    , archive_path(std::move(other.archive_path))
    , file_path(std::move(other.file_path))
    , raw_file_entry(std::move(other.raw_file_entry))
{
}

ImageFile::~ImageFile()
{
    raw_file_entry.clear();
}

ImageFile &
ImageFile::operator=(const ImageFile &other)
{
    ft = other.ft;
    archive_path = other.archive_path;
    file_path = other.file_path;
    raw_file_entry = other.raw_file_entry;
    return *this;
}

ImageFile &
ImageFile::operator=(ImageFile &&other)
{
    ft = other.ft;
    archive_path = std::move(other.archive_path);
    file_path = std::move(other.file_path);
    raw_file_entry = std::move(other.raw_file_entry);
    return *this;
}

ImageFile::FileType
ImageFile::fileType() const
{
    return ft;
}

bool
ImageFile::isValid() const
{
    return fileType() != INVALID;
}

QString
ImageFile::physicalFilePath() const
{
    switch (ft)
    {
        case ARCHIVE: return archive_path;
        case RAW:     return file_path;
        default:      return QString();
    }
}

QString
ImageFile::physicalFileName() const
{
    return QFileInfo(physicalFilePath()).fileName();
}

QString
ImageFile::logicalFilePath() const
{
    switch (ft)
    {
        case ARCHIVE: return file_path;
        case RAW:     return file_path;
        default:      return QString();
    }
}

QString
ImageFile::logicalFileName() const
{
    return QFileInfo(logicalFilePath()).fileName();
}

const QByteArray&
ImageFile::rawFilePath() const
{
    return raw_file_entry;
}

QString
ImageFile::createKey() const
{
    return logicalFilePath();
}

QByteArray *
ImageFile::readData() const
{
    switch (fileType())
    {
        case RAW:     return readImageData();
        case ARCHIVE: return readArchiveData();
        case INVALID: return nullptr;
    }
    return nullptr;
}

bool
ImageFile::isReadableImageFile(const QString &path)
{
    int len = sizeof(readable_image_suffix) / 
        sizeof(readable_image_suffix[0]);
    QString ext = QFileInfo(path).suffix();
    for (int i = 0; i < len; ++i)
    {
        if (ext == readable_image_suffix[i]) return true;
    }
    return false;
}

bool
ImageFile::isReadableArchiveFile(const QString &path)
{
    int len = sizeof(readable_archive_suffix) / 
        sizeof(readable_archive_suffix[0]);
    QString ext = QFileInfo(path).suffix();
    for (int i = 0; i < len; ++i)
    {
        if (ext == readable_archive_suffix[i]) return true;
    }
    return false;
}

const QString &
ImageFile::readableFormatExt()
{
    static QString extlist;
    if (extlist.isNull())
    {
        int len_i = sizeof(readable_image_suffix) / 
            sizeof(readable_image_suffix[0]);
        extlist = "Images (";
        for (int i = 0; i < len_i; ++i)
        {
            extlist += QString("*.%1 ")
                .arg(readable_image_suffix[i]);
        }
        extlist += ")\n";

        int len_a = sizeof(readable_archive_suffix) / 
            sizeof(readable_archive_suffix[0]);
        extlist += "Archives (";
        for (int i = 0; i < len_a; ++i)
        {
            extlist += QString("*.%1 ")
                .arg(readable_archive_suffix[i]);
        }
        extlist += ")\nAllFiles (*.*)";
    }
    return extlist;
}

QVector<ImageFile*>
ImageFile::openArchive(const QString &path)
{
    QVector<ImageFile*> files;
    if (!isReadableArchiveFile(path)) return files;

    struct archive *a;

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    int r = archive_read_open_filename(a,
            path.toLocal8Bit().constData(), 1024*128);
    if (r != ARCHIVE_OK)
    {
        fprintf(stderr, "%s\n", archive_error_string(a));
        archive_read_free(a);
        return files;
    }

    struct archive_entry *ae;
    while (archive_read_next_header(a, &ae) == ARCHIVE_OK)
    {
        QByteArray raw_entry(archive_entry_pathname(ae));
        QString entry_name(raw_entry);
        if (ImageFile::isReadableImageFile(entry_name))
        {
            archive_read_data_skip(a);
            ImageFile *imgfile = new ImageFile(path, raw_entry);
            files << imgfile;
        }
    }

    r = archive_read_free(a);
    if (r != ARCHIVE_OK)
    {
        fprintf(stderr, "%s\n", archive_error_string(a));
    }
    return files;
}

QByteArray *
ImageFile::readImageData() const
{
    QFile file(physicalFilePath());
    if (file.open(QIODevice::ReadOnly))
    {
        return new QByteArray(file.readAll());
    }
    return nullptr;
}

QByteArray *
ImageFile::readArchiveData() const
{
    struct archive *a;

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    int r = archive_read_open_filename(a,
            physicalFilePath().toLocal8Bit().constData(),
            1024*128);
    if (r != ARCHIVE_OK)
    {
        fprintf(stderr, "%s\n", archive_error_string(a));
        archive_read_free(a);
        return nullptr;
    }

    struct archive_entry *ae;
    const QByteArray &s_entry = rawFilePath();
    while (archive_read_next_header(a, &ae) == ARCHIVE_OK)
    {
        QByteArray entry(archive_entry_pathname(ae));
        if (s_entry == entry)
        {
            QByteArray *data = new QByteArray();
            const void *buf;
            size_t len;
            la_int64_t offset;

            while (archive_read_data_block(a, &buf, &len, &offset)
                    == ARCHIVE_OK)
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

