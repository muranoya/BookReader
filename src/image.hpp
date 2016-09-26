#ifndef IMAGE_H
#define IMAGE_H
#include <QImage>

/* Nearest Neighbor */
QImage nn(const QImage &src, const double s);
/* Bilinear */
QImage bl(const QImage &src, const double s);
/* Bicubic */
QImage bc(const QImage &src, const double s);

#endif // IMAGE_H
