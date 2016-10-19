#ifndef PLAYLISTMODEL_HPP
#define PLAYLISTMODEL_HPP

#include <QAbstractListModel>
#include <QModelIndex>
#include <QStringList>
#include <QAbstractItemView>
#include <QTextCodec>
#include <QImage>
#include <QItemSelectionModel>
#include <QVector>
#include "ImageFile.hpp"
#include "Prefetcher.hpp"

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit PlaylistModel(QObject *parent = 0);
    ~PlaylistModel();

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    void showSelectedItem();
    void removeSelectedItem();
    void clearPlaylist();

    void setOpenDirLevel(int n);
    int getOpenDirLevel() const;

    void setCacheSize(int n);
    int getCacheSize() const;

    int countShowImages() const;
    int count() const;
    bool empty() const;

    int currentIndex(int i) const;
    QString currentFileName(int i) const;

    void setModelToItemView(QAbstractItemView *view);

public slots:
    void openImages(const QStringList &path);
    void nextImage();
    void prevImage();
    void changeNumOfImages(int n);

signals:
    void changeImages(const QImage &img_l, const QImage &img_r);
    void changePlaylistStatus();

private slots:
    void itemViewDoubleClicked(const QModelIndex &index);

private:
    QItemSelectionModel *slct;
    QVector<ImageFile*> files;
    int opendirlevel;
    int img_index;
    int img_num;
    Prefetcher prft;

    int nextIndex(int idx, int c) const;
    bool isValidIndex(int i) const;
    bool isCurrentIndex(int i) const;

    void dataChangeNotice(int newidx, int newnum);
    int openFilesAndDirs(const QStringList &paths, int level);
    void openFilesAndDirs0(QVector<ImageFile*> &openfiles,
            const QStringList &paths, int level);

    void showImages();
    QImage loadData(const ImageFile &f);
};

#endif // PLAYLISTMODEL_HPP
