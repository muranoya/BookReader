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

    void setSpreadView(bool spread);
    bool getSpreadView() const;

    void setRightbindingView(bool rbind);
    bool getRightbindingView() const;

    void setAutoAdjustSpread(bool aas);
    bool getAutoAdjustSpread() const;

    void setFeedPageMode(FeedPageMode mode);
    FeedPageMode getFeedPageMode() const;

    QSize getImageSize() const;

signals:
    void nextImageRequest();
    void prevImageRequest();
    void changeNumOfImages(int n);
    void openImageFiles(const QStringList &paths);

protected slots:
    void showImages(const QImage &img_l, const QImage &img_r);

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
    QImage based_imgs[2];   // 表示している画像
    QImage scaled_imgs[2];  // スケール後の画像
    int img_num;
    ScalingMode scale_mode; // 画素補完方法
    double scale_factor;    // 表示倍率
    ViewMode view_mode;     // 表示方法
    bool spread_view;
    bool rbind_view;
    bool autospread;
    FeedPageMode fp_mode;   // ページ遷移モード
    QTimer drag_timer;      // ドラッグかクリックかの判定用タイマー
    bool is_drag_img;       // ドラッグ判定のときtrue
    QPoint click_pos;       // クリックした位置
    QPoint click2_pos;      // 
    QPoint move_pos;        // 移動中の位置
    QPoint img_pos;         // 画像の表示位置

    // ドラッグと判定するまでの時間(ms)
    const int drag_detect_time;

    void rescaling();
};

#endif // VIEWER_HPP
