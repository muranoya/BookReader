#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include "nullptr.h"
#include <QImage>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QStringList>
#include <QGraphicsView>
#include <QDir>
#include <QMessageBox>
#include <QString>
#include <QFileInfo>
#include <QSize>
#include <QPainter>

class ImageManager
{
public:
    ImageManager(QGraphicsView *view);
    ~ImageManager();

    // 画像を読み込む
    // 正常に読み込めればtrue, それ以外の時false
    bool loadFile(const QString &path);
    // フォルダに含まれる全ての扱える画像をリストに加え、先頭のファイルを読み込む。
    // 正常に読み込めればtrue, それ以外の時false
    bool loadDir(const QString &path);

    // 画像のプレイリストで次の画像へ移動する
    void nextImage();
    // 画像のプレイリストで前の画像へ移動する
    void previousImage();

    // 読み込んでいる画像を開放する
    void releaseImages();

    // 画像のプレイリストを返す
    QStringList getImageList();
    // 画像のプレイリスト数を返す
    int getImageListCount();

    // 読み込み可能な画像ファイルの拡張子を返す
    QStringList getReadableExtension();
    // 指定したファイルパスが読み込み可能な拡張子か調べる
    // 読み込み可能な拡張子の場合true, それ以外の時false
    bool isReadable(const QString &path);

    // 現在表示している画像のインデックスを返す
    int getImageListIndex();
    QString getShowingFileName();
    QSize getShowingImageSize();

private:
    QGraphicsView *view;

    QImage *view_img;
    QGraphicsScene *view_gscene;
    QGraphicsPixmapItem *view_gitem;

    // 対応している拡張子
    QString extList[5] = {"jpg", "png", "jpeg", "bmp", "gif"};
    const int extListLen = 5;

    // 画像のプレイリスト。画像ファイルへの完全パスが入る
    QStringList imgList;
    // 表示中のimgListのインデックス。
    // 負数で何も表示していないことを示す
    int showingIndex;

    // 画像を表示する
    // 表示できた時true, それ以外の時false
    bool showImage(int n);

    // パスを連結する
    QString connectFilePath(QString parent, QString child);
};

#endif // IMAGEMANAGER_H
