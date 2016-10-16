#include <QPainter>
#include <QMimeData>
#include <QFileInfo>
#include "image.hpp"
#include "Viewer.hpp"

#include "for_windows_env.hpp"

Viewer::Viewer(QWidget *parent)
    : QWidget(parent)
    , based_img()
    , scaled_img()
    , scale_mode(Bilinear)
    , scale_factor(1.0)
    , view_mode(FittingWindow)
    , fp_mode(MouseButton)
    , drag_timer()
    , is_drag_img(false)
    , click_pos()
    , click2_pos()
    , move_pos()
    , img_pos()
    , drag_detect_time(150)
{
    connect(&drag_timer, SIGNAL(timeout()), this, SLOT(drag_check()));

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
        if (c)
        {
            rescaling();
            emit changeViewMode();
        }
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
    return based_img.size();
}

void
Viewer::showImage(const QImage &img)
{
    based_img = img;

    if (img.isNull())
    {
        scaled_img = QImage();
    }
    else
    {
        scaleImage();
    }

    int x = width() - scaled_img.width();
    int y = height() - scaled_img.height();
    x = (x < 0 ? 0 : x/2);
    y = (y < 0 ? 0 : y/2);
    img_pos = QPoint(x, y);
    update();
}

void
Viewer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::CrossPattern);

    if (scaled_img.isNull()) return;

    switch (getViewMode())
    {
        case FittingWindow:
        {
            int x = std::max(width()  - scaled_img.width(),  0);
            int y = std::max(height() - scaled_img.height(), 0);
            painter.drawImage(QPoint(x/2, y/2), scaled_img);
            break;
        }
        case FittingWidth:
        {
            int x = std::max(width() - scaled_img.width(), 0);
            img_pos += move_pos - click2_pos;
            click2_pos = move_pos;
            painter.drawImage(QPoint(x/2, img_pos.y()), scaled_img);
            break;
        }
        default:
            img_pos += move_pos - click2_pos;
            click2_pos = move_pos;
            painter.drawImage(img_pos, scaled_img);
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
    for (QList<QUrl>::const_iterator iter = urls.constBegin();
            iter != urls.constEnd(); ++iter)
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
    if (!based_img.isNull()) scaleImage();
}

void
Viewer::scaleImage()
{
    double scale = 1.0;

    if (based_img.isNull()) return;

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
        if (width() < based_img.width())
        {
            ws = width() / static_cast<double>(based_img.width());
        }

        if (height() < based_img.height())
        {
            hs = height() / static_cast<double>(based_img.height());
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
        scaled_img = based_img;
    }
    else
    {
        QImage (*f[])(const QImage &, const double) = {nn, bl, bc};
        scaled_img = f[scale_mode](based_img, scale_factor);
    }
    update();
}

