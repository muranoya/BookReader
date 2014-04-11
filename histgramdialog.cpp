#include "histgramdialog.h"

HistgramDialog::HistgramDialog(QWidget *parent)
    : QDialog(parent,
              Qt::Drawer |
              Qt::WindowTitleHint |
              Qt::WindowCloseButtonHint |
              Qt::WindowStaysOnTopHint),
      baselayout(new QVBoxLayout()),
      view(new QLabel()),
      graph_w(277),
      graph_h(135),
      margin_top(5),
      margin_bottom(5),
      margin_left(15),
      margin_right(5)
{
    setWindowTitle(tr("ヒストグラム"));

    baselayout->setMargin(0);
    setLayout(baselayout);
    baselayout->addWidget(view);
}

HistgramDialog::~HistgramDialog()
{
    delete baselayout;
    delete view;
}

void HistgramDialog::setHistgram(const QVector<int> &data)
{
    if (data.size() != 256*3)
    {
        return;
    }

    // 左に15pixel, 右に5pixel
    // 上に5pixel, 下に5pixelの余裕を持たせたサイズ
    QPixmap img(graph_w, graph_h*3);

    int max = 0;
    for (int i(0); i < 256*3; ++i)
    {
        if (max < data[i])
        {
            max = data[i];
        }
    }
    qreal s = max / qreal(graph_h-margin_top-margin_bottom-1);

    paintHistgram(0, graph_h*0, graph_w, graph_h, img, QColor(Qt::red),   data, 256*0, s);
    paintHistgram(0, graph_h*1, graph_w, graph_h, img, QColor(Qt::green), data, 256*1, s);
    paintHistgram(0, graph_h*2, graph_w, graph_h, img, QColor(Qt::blue),  data, 256*2, s);

    view->setPixmap(img);
    view->resize(img.size());
    setFixedSize(view->size());

    show();
}

void HistgramDialog::releaseHistgramImage()
{
    baselayout->removeWidget(view);
    delete view;

    view = new QLabel();
    baselayout->addWidget(view);
}

void HistgramDialog::closeEvent(QCloseEvent *e)
{
    Q_UNUSED(e);
    releaseHistgramImage();
}

// x,yはグラフの左上の点を、w,hはグラフの大きさを。
void HistgramDialog::paintHistgram(int x, int y, int w, int h,
                                   QPaintDevice &drawing,
                                   const QColor &graph,
                                   const QVector<int> &data,
                                   int start, int s) const
{
    /*
     * 描画する画像の原点(0,0)は左上。
     * つまり、上端がX軸で、右上がX軸正方向、
     * 左端がY軸で、左下がY軸正方向となる。
     *
     * 通常のグラフのように、左下を原点とし、下端をX軸、右下をX軸正方向、
     * 左端をY軸、左上をY軸正方向として描画しようとすると、
     * 上下が逆さまになるので注意が必要。
     */
    QPainter painter;
    QPen blackpen(QBrush(Qt::black), 1);
    QPen graphpen(QBrush(graph), 1);

    painter.begin(&drawing);
    {
        // 背景の描画
        painter.fillRect(x, y, w, h, Qt::white);

        // グラフ枠の描画
        painter.setPen(blackpen);
        // X軸
        painter.drawLine(x+margin_left, y+margin_top, x+margin_left, y+h-margin_bottom);
        // Y軸
        painter.drawLine(x+margin_left, y+h-margin_bottom, x+w-margin_right, y+h-margin_bottom);

        // グラフの中身の描画
        painter.setPen(graphpen);
        for (int i(0); i < 256; ++i)
        {
            if (data[start+i] > 0)
            {
                painter.drawLine(x+i+margin_left+1,
                                 y+h-margin_bottom-1,
                                 x+i+margin_left+1,
                                 y+h-margin_bottom-1-(data[start+i]/s));
            }
        }
    }
    painter.end();
}
