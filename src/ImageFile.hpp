#ifndef IMAGEFILE_HPP
#define IMAGEFILE_HPP

#include <QString>
#include <QByteArray>
#include <QTextCodec>
#include <QVector>

class ImageFile
{
public:
    enum FileType
    {
        INVALID,
        RAW,
        ARCHIVE,
    };
    explicit ImageFile();
    explicit ImageFile(const QString &path, const QByteArray &entry);
    explicit ImageFile(const QString &path);
    ImageFile(const ImageFile &other);
    ImageFile(ImageFile &&other);
    virtual ~ImageFile();

    ImageFile &operator=(const ImageFile &other);
    ImageFile &operator=(ImageFile &&other);

    FileType fileType() const;
    bool isValid() const;
    
    QString physicalFilePath() const;
    QString physicalFileName() const;
    QString logicalFilePath() const;
    QString logicalFileName() const;

    const QByteArray &rawFilePath() const;
    QString createKey() const;

    QByteArray *readData() const; // for prefetcher

    static bool isReadableImageFile(const QString &path);
    static bool isReadableArchiveFile(const QString &path);
    static const QString &readableFormatExt();
    static QVector<ImageFile*> openArchive(const QString &path);

private:
    FileType ft;
    QString archive_path;
    QString file_path;
    QByteArray raw_file_entry;

    QByteArray *readImageData() const;
    QByteArray *readArchiveData() const;
};

#endif // IMAGEFILE_HPP
