#include <QPainter>
#include <QMimeData>
#include <QFileInfo>
#include "image.hpp"
#include "Viewer.hpp"

#include "for_windows_env.hpp"

Viewer::Viewer(QWidget *parent)
    : QWidget(parent)
    , based_imgs()
    , scaled_imgs()
    , img_num(0)
    , scale_mode(Bilinear)
    , scale_factor(1.0)
    , view_mode(FittingWindow)
    , spread_view(false)
    , rbind_view(false)
    , autospread(false)
    , fp_mode(MouseButton)
    , drag_timer()
    , is_drag_img(false)
    , click_pos()
    , click2_pos()
    , move_pos()
    , img_pos()
    , drag_detect_time(150)
{
    connect(&drag_timer, SIGNAL(timeout()),
            this, SLOT(drag_check()));

    setFocusPolicy(Qt::StrongFocus);
    setAcceptDrops(true);
}

Viewer::~Viewer(){}

void
Viewer::setScalingMode(ScalingMode mode)
{
    bool c = (mode != scale_mode);
    scale_mode = mode;
    if (c) rescaling();
}

Viewer::ScalingMode
Viewer::getScalingMode() const
{
    return scale_mode;
}

void
Viewer::setViewMode(Viewer::ViewMode mode, double factor)
{
    bool c = (mode != view_mode || factor != scale_factor);
    if (0.01 <= factor && factor <= 10)
    {
        view_mode = mode;
        scale_factor = factor;
        if (c) rescaling();
    }
}

void
Viewer::setViewMode(Viewer::ViewMode mode)
{
    setViewMode(mode, scale_factor);
}

double
Viewer::getCustomScaleFactor() const
{
    return scale_factor;
}

Viewer::ViewMode
Viewer::getViewMode() const
{
    return view_mode;
}

void
Viewer::setSpreadView(bool spread)
{
    bool c = (spread_view != spread);
    spread_view = spread;
    if (c) rescaling();
}

bool
Viewer::getSpreadView() const
{
    return spread_view;
}

void
Viewer::setRightbindingView(bool rbind)
{
    bool c = (rbind_view != rbind);
    rbind_view = rbind;
    if (c) rescaling();
}

bool
Viewer::getRightbindingView() const
{
    return rbind_view;
}

void
Viewer::setAutoAdjustSpread(bool aas)
{
    bool c = (autospread != aas);
    autospread = aas;
    if (c) rescaling();
}

bool
Viewer::getAutoAdjustSpread() const
{
    return autospread;
}

void
Viewer::setFeedPageMode(Viewer::FeedPageMode mode)
{
    fp_mode = mode;
}

Viewer::FeedPageMode
Viewer::getFeedPageMode() const
{
    return fp_mode;
}

QSize
Viewer::getImageSize() const
{
    int w = 0;
    int h = 0;
    for (int i = 0; i < img_num; ++i)
    {
        w += based_imgs[i].width();
        h = std::max(h, based_imgs[i].height());
    }
    return QSize(w, h);
}

void
Viewer::showImages(const QImage &imgl, const QImage &imgr)
{
    based_imgs[0] = imgl;
    based_imgs[1] = imgr;
    img_pos = QPoint(0, 0);
    rescaling();
}

void
Viewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::CrossPattern);

    if (img_num == 0) return;

    QImage *imgs[2] = {scaled_imgs+0, scaled_imgs+1};
    if (img_num == 2 && getRightbindingView())
    {
        QImage *img_p = imgs[0];
        imgs[0] = imgs[1];
        imgs[1] = img_p;
    }

    int cimg_w = imgs[0]->width();
    int cimg_h = imgs[0]->height();
    if (img_num == 2)
    {
        cimg_w += imgs[1]->width();
        cimg_h = std::max(cimg_h, imgs[1]->height());
    }

    switch (getViewMode())
    {
        case ActualSize:
        {
            img_pos += move_pos - click2_pos;
            click2_pos = move_pos;
            QPoint pos(img_pos.x(),
                    img_pos.y() + (cimg_h - imgs[0]->height())/2);
            painter.drawImage(pos, *imgs[0]);
            if (img_num == 2)
            {
                pos = QPoint(pos.x() + imgs[0]->width(),
                        img_pos.y() + (cimg_h - imgs[1]->height())/2);
                painter.drawImage(pos, *imgs[1]);
            }
        }
        break;
        case FittingWindow:
        {
            QPoint pos((width() - cimg_w)/2,
                    (height() - imgs[0]->height())/2);
            painter.drawImage(pos, *imgs[0]);
            if (img_num == 2)
            {
                pos = QPoint(pos.x() + imgs[0]->width(),
                        (height() - imgs[1]->height())/2);
                painter.drawImage(pos, *imgs[1]);
            }
        }
        break;
        case FittingWidth:
        {
            img_pos += move_pos - click2_pos;
            click2_pos = move_pos;
            QPoint pos((width() - cimg_w)/2,
                    img_pos.y() + (height() - imgs[0]->height())/2);
            painter.drawImage(pos, *imgs[0]);
            if (img_num == 2)
            {
                pos = QPoint(pos.x() + imgs[0]->width(),
                        img_pos.y() + (height() - imgs[1]->height())/2);
                painter.drawImage(pos, *imgs[1]);
            }
        }
        break;
        case CustomScale:
        {
            img_pos += move_pos - click2_pos;
            click2_pos = move_pos;
            QPoint pos(img_pos.x(),
                    img_pos.y() + (cimg_h - imgs[0]->height())/2);
            painter.drawImage(pos, *imgs[0]);
            if (img_num == 2)
            {
                pos = QPoint(pos.x() + imgs[0]->width(),
                        img_pos.y() + (cimg_h - imgs[1]->height())/2);
                painter.drawImage(pos, *imgs[1]);
            }
        }
        break;
    }
}

void
Viewer::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
    if (event->key() == Qt::Key_Right) emit nextImageRequest();
    if (event->key() == Qt::Key_Left)  emit prevImageRequest();
}

void
Viewer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    rescaling();
}

void
Viewer::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void
Viewer::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();
    const QList<QUrl> urls = mime->urls();

    QStringList list;
    for (auto iter = urls.cbegin();
            iter != urls.cend(); ++iter)
    {
        const QString path = iter->toLocalFile();
        if (QFileInfo(path).exists()) list << path;
    }

    if (!list.empty()) emit openImageFiles(list);
}

void
Viewer::mousePressEvent(QMouseEvent *event)
{
    is_drag_img = false;
    switch (getFeedPageMode())
    {
        case MouseButton:
            if (event->buttons() & Qt::RightButton)
            {
                emit prevImageRequest();
            }
            else if (event->buttons() & Qt::LeftButton)
            {
                if (getViewMode() == FittingWindow)
                {
                    emit nextImageRequest();
                }
                else
                {
                    move_pos = click_pos = click2_pos = event->pos();
                    drag_timer.start(drag_detect_time);
                }
            }
            break;
        case MouseClickPosition:
            if (event->buttons() & Qt::LeftButton)
            {
                if (getViewMode() == FittingWindow)
                {
                    if (event->pos().x() < width()/2)
                    {
                        emit prevImageRequest();
                    }
                    else
                    {
                        emit nextImageRequest();
                    }
                }
                else
                {
                    move_pos = click_pos = click2_pos = event->pos();
                    drag_timer.start(drag_detect_time);
                }
            }
            break;
    }
    event->accept();
}

void
Viewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (getViewMode() != FittingWindow &&
            event->button() == Qt::LeftButton && !is_drag_img)
    {
        switch (getFeedPageMode())
        {
            case MouseClickPosition:
                if (event->pos().x() < width()/2)
                {
                    emit prevImageRequest();
                }
                else
                {
                    emit nextImageRequest();
                }
                break;
            case MouseButton:
                emit nextImageRequest();
                break;
        }
    }
    event->accept();
}

void
Viewer::mouseMoveEvent(QMouseEvent *event)
{
    if (getViewMode() != FittingWindow)
    {
        move_pos = event->pos();
        update();
    }
    event->accept();
}

void
Viewer::drag_check()
{
    is_drag_img = true;
    drag_timer.stop();
}

void
Viewer::rescaling()
{
    double scale = 1.0;
    int cimg_w = 0;
    int cimg_h = 0;
    int old_imgnum = img_num;

    scaled_imgs[0] = QImage();
    scaled_imgs[1] = QImage();

    if (getSpreadView() &&
            !based_imgs[0].isNull() &&
            !based_imgs[1].isNull())
    {
        img_num = 2;
        cimg_w = based_imgs[0].width() + based_imgs[1].width();
        cimg_h = std::max(
                based_imgs[0].height(),
                based_imgs[1].height());
        if (getAutoAdjustSpread())
        {
            double v_wh = width() / static_cast<double>(height());
            int img1_w = based_imgs[0].width();
            int img1_h = based_imgs[0].height();
            double img1_wh = img1_w / static_cast<double>(img1_h);
            double img2_wh = cimg_w /
                static_cast<double>(cimg_h);
            if (std::fabs(v_wh - img1_wh) < std::fabs(v_wh - img2_wh))
            {
                img_num = 1;
                cimg_w = img1_w;
                cimg_h = img1_h;
            }
        }
    }
    else if (!based_imgs[0].isNull())
    {
        img_num = 1;
        cimg_w = based_imgs[0].width();
        cimg_h = based_imgs[0].height();
    }
    else
    {
        img_num = 0;
        update();
        return;
    }

    if (getViewMode() == CustomScale)
    {
        scale = scale_factor;
    }
    else if (getViewMode() == ActualSize)
    {
        scale = 1.0;
    }
    else
    {
        double ws = 1.0;
        double hs = 1.0;
        if (width() < cimg_w)
        {
            ws = width() / static_cast<double>(cimg_w);
        }

        if (height() < cimg_h)
        {
            hs = height() / static_cast<double>(cimg_h);
        }

        if (getViewMode() == FittingWindow)
        {
            scale = ws > hs ? hs : ws;
        }
        else if (getViewMode() == FittingWidth)
        {
            scale = ws;
        }
    }
    scale_factor = scale;

    if (qFuzzyCompare(scale_factor, 1.0))
    {
        scaled_imgs[0] = based_imgs[0];
        scaled_imgs[1] = based_imgs[1];
    }
    else
    {
        QImage (*f[])(const QImage &, const double) = {nn, bl, bc};
        for (int i = 0; i < img_num; ++i)
        {
            scaled_imgs[i] = f[scale_mode](based_imgs[i], scale_factor);
        }
    }
    if (old_imgnum != img_num) emit changeNumOfImages(img_num);
    update();
}

