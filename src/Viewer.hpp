#ifndef VIEWER_HPP
#define VIEWER_HPP

#include <QWidget>
#include <QSize>
#include <QImage>
#include <QTimer>
#include <QPaintEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QPoint>

class Viewer : public QWidget
{
    Q_OBJECT
public:
    enum FeedPageMode
    {
        MouseClickPosition,
        MouseButton,
    };
    enum ViewMode
    {
        ActualSize,
        FittingWindow,
        FittingWidth,
        CustomScale,
    };
    enum ScalingMode
    {
        NearestNeighbor,
        Bilinear,
        Bicubic,
    };
    
    explicit Viewer(QWidget *parent = 0);
    ~Viewer();

    void setScalingMode(ScalingMode mode);
    ScalingMode getScalingMode() const;

    void setViewMode(ViewMode mode, double factor);
    void setViewMode(ViewMode mode);
    double getCustomScaleFactor() const;
    ViewMode getViewMode() const;

    void setFeedPageMode(FeedPageMode mode);
    FeedPageMode getFeedPageMode() const;

    QSize getImageSize() const;

signals:
    // 次の画像への遷移要求
    void nextImageRequest();
    // 前の画像への遷移要求
    void prevImageRequest();
    // 指定したファイルパスを開く要求
    void openImageFiles(const QStringList &paths);
    // 画像表示状態が変わったとき
    void changeViewMode();

public slots:
    void showImage(const QImage &img);

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private slots:
    void drag_check();

private:
    QImage based_img;       // 表示している画像
    QImage scaled_img;      // スケール後の画像
    ScalingMode scale_mode; // 画素補完方法
    double scale_factor;    // 表示倍率
    ViewMode view_mode;     // 表示方法
    FeedPageMode fp_mode;   // ページ遷移モード
    QTimer drag_timer;      // ドラッグかクリックかの判定用タイマー
    bool is_drag_img;       // ドラッグ判定のときtrue
    QPoint click_pos;       // クリックした位置
    QPoint click2_pos;      // 
    QPoint move_pos;        // 移動中の位置
    QPoint img_pos;         // 画像の表示位置

    const int drag_detect_time; // ドラッグと判定するまでの時間(ms)

    void rescaling();
    void scaleImage();
};

#endif // VIEWER_HPP
