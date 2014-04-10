#ifndef HISTGRAMDIALOG_H
#define HISTGRAMDIALOG_H

#include "nullptr.h"

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
    void releaseHistgramImage();

protected:
    virtual void closeEvent(QCloseEvent *e);

private:
    QVBoxLayout *baselayout;
    QLabel *view;
    QPixmap *img;

    int graph_w;
    int graph_h;
    int margin_top;
    int margin_bottom;
    int margin_left;
    int margin_right;

    // リストには[R,G,B,A]のヒストグラム値を入れる
    void paintHistgram(int x, int y, int w, int h,
                       QPaintDevice &img,
                       const QColor &graph,
                       const QVector<int> &data,
                       int start, int s) const;
};

#endif // HISTGRAMDIALOG_H
