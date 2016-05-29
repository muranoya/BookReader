#ifndef HISTGRAMDIALOG_H
#define HISTGRAMDIALOG_H

#include <QDialog>
#include <QPixmap>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>

class HistgramDialog : public QDialog
{
    Q_OBJECT
public:
    explicit HistgramDialog(QWidget *parent = 0);
    ~HistgramDialog();

    void setHistgram(const QVector<int> &data);
    void releaseHistgram();

signals:
    void closeDialog();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    QVBoxLayout *baselayout;
    QLabel *view;

    const int graph_w;
    const int graph_h;
    const int margin_top;
    const int margin_bottom;
    const int margin_left;
    const int margin_right;

    // リストには[R,G,B]のヒストグラム値を入れる
    void paintHistgram(const int x, const int y, const int w, const int h,
                       QPaintDevice &img,
                       const QColor &graph,
                       const QVector<int> &data,
                       const int start, const qreal s) const;
};

#endif // HISTGRAMDIALOG_H
