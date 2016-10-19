#ifndef IMAGEVIEWER_HPP
#define IMAGEVIEWER_HPP

#include <QWidget>
#include <QStringList>
#include <QAbstractItemView>
#include "Viewer.hpp"
#include "PlaylistModel.hpp"

class ImageViewer : public Viewer
{
    Q_OBJECT
public:
    explicit ImageViewer(QWidget *parent = 0);
    ~ImageViewer();

    void openImages(const QStringList &paths);

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
    void showSelectedItem();
    void removeSelectedItem();
    void clearPlaylist();

signals:
    void changeImageViewerStatus();

protected slots:
    void changedStatus();

private:
    PlaylistModel plmodel;

};

#endif //IMAGEVIEWER_HPP
