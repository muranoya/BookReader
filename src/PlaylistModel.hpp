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

    void setSpreadView(bool spread);
    bool getSpreadView() const;

    void setRightbindingView(bool rb);
    bool getRightbindingView() const;

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

signals:
    void changeImage(const QImage &img);
    void changePlaylistStatus();

public slots:
    void openImages(const QStringList &path);
    void showSelectedItem();
    void removeSelectedItem();
    void clearPlaylist();
    void nextImage();
    void prevImage();

private slots:
    void itemViewDoubleClicked(const QModelIndex &index);

private:
    QItemSelectionModel *slct;
    QVector<ImageFile*> files;
    bool spreadview;
    bool rbindview;
    int opendirlevel;
    int img_index;
    Prefetcher *prft;

    int nextIndex(int idx, int c) const;
    bool isValidIndex(int i) const;
    bool isCurrentIndex(int i) const;

    void dataChangeNotice(int newidx);
    void openFilesAndDirs(const QStringList &paths, int level);
    void openFilesAndDirs0(QVector<ImageFile*> &openfiles,
            const QStringList &paths, int level);

    QImage loadData(const ImageFile &f);
    QImage combineImage();
};

#endif // PLAYLISTMODEL_HPP
