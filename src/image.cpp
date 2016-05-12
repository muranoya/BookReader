#include "image.hpp"
#include <cmath>

/* Nearest Neighbor */
QImage
nn(const QImage src, const qreal s)
{
    const int w = src.width();
    const int h = src.height();
    const int nw = w*s;
    const int nh = h*s;
    const int x1 = w-1;
    const int y1 = h-1;

    QImage nimg(nw, nh, src.format());
    QRgb *nbits = (QRgb*)nimg.bits();
    const QRgb *bits = (QRgb*)src.bits();

    for (int y = 0; y < nh; ++y)
    {
        const int y0 = std::min(int(std::floor(y/s+0.5)), y1)*w;
        for (int x = 0; x < nw; ++x)
        {
            *(nbits+x) = *(bits+y0+std::min(int(std::floor(x/s+0.5)), x1));
        }
        nbits += nw;
    }

    return nimg;
}

/* Bilinear */
QImage
bl(const QImage src, const qreal s)
{
    const int w = src.width();
    const int h = src.height();
    const int nw = w*s;
    const int nh = h*s;
    const int w1 = w-1;
    const int h1 = h-1;

    //struct timeval tp[2];
    //gettimeofday(tp, NULL);

    QImage nimg(nw, nh, src.format());
    QRgb *nbits = (QRgb*)nimg.bits();
    const QRgb *bits = (QRgb*)src.bits();

    // [x], (x-[x])を計算しておく
    int icache[nw];
    qreal dcache[nw];
    for (int x = 0; x < nw; ++x)
    {
        const qreal x0 = x/s;          // x;
        const int xg = std::floor(x0); // [x];
        icache[x] = std::min(xg, w1);  // xg
        dcache[x] = x0-xg;             // x-[x]
    }

    // ディジタル画像処理 CG-ARTS協会 2012年第2版の以下の式
    // I(x, y) =
    // ([x]+1-x) ([y]+1-y) f([x], [y])   +
    // ([x]+1-x) (y-[y])   f([x], [y]+1) +
    // (x-[x])   ([y]+1-y) f([x]+1, [y]) +
    // (x-[x])   (y-[y])   f([x]+1, [y]+1)
    for (int y = 0; y < nh; ++y)
    {
        const qreal y0 = y/s;          // y
        const int yg = std::floor(y0); // [y]

        const qreal ty0 = y0-yg; // y-[y]
        const qreal ty1 = 1-ty0; // [y]+1-y
        const int yi0 = std::min(yg, h1)*w;
        const int yi1 = std::min(yg+1, h1)*w;

        for (int x = 0; x < nw; ++x)
        {
            const qreal d = dcache[x];
            const qreal t1 = (1.-d)*ty1; //([x]+1-x)([y]+1-y)
            const qreal t2 = (1.-d)*ty0; //([x]+1-x)(y-[y])
            const qreal t3 = d*ty1;      //(x-[x])([y]+1-y)
            const qreal t4 = d*ty0;      //(x-[x])(y-[y])

            const int i = icache[x];
            const QRgb p00 = *(bits+i  +yi0);
            const QRgb p10 = *(bits+i+1+yi0);
            const QRgb p01 = *(bits+i  +yi1);
            const QRgb p11 = *(bits+i+1+yi1);

            *(nbits+x) = qRgba(
                    t1*qRed(p00)   + t2*qRed(p01)   + t3*qRed(p10)   + t4*qRed(p11),
                    t1*qGreen(p00) + t2*qGreen(p01) + t3*qGreen(p10) + t4*qGreen(p11),
                    t1*qBlue(p00)  + t2*qBlue(p01)  + t3*qBlue(p10)  + t4*qBlue(p11),
                    t1*qAlpha(p00) + t2*qAlpha(p01) + t3*qAlpha(p10) + t4*qAlpha(p11));
        }
        nbits += nw;
    }
    //gettimeofday(tp+1, NULL);
    //fprintf(stderr, "bilinear: %f\n", elapsed_time(tp));

    return nimg;
}

static qreal
bicubic_h(const qreal t)
{
    const qreal u = std::fabs(t);
    if (u <= 1)
    {
        // abs(t) <= 1の時
        // (a+2)*abs(t)^3 - (a+3)*abs(t)^2 + 1
        return (u*u*u)-2*(u*u)+1;
    }
    else if (1 < u && u <= 2)
    {
        // 1 < t <= 2の時
        // a*abs(t)^3 - 5a*abs(t)^2 + 8a*abs(t) - 4a
        return -(u*u*u)+5*(u*u)-8*u+4;
    }
    // 2 < abs(t)のとき
    return 0;
}

static int
bicubic_matmul(const qreal d1[4], const int d2[4][4], const qreal d3[4])
{
    qreal temp[4];
    for (int i = 0; i < 4; ++i)
    {
        temp[i] = d1[0]*d2[0][i]
                + d1[1]*d2[1][i]
                + d1[2]*d2[2][i]
                + d1[3]*d2[3][i];
    }
    return int(temp[0]*d3[0]
              +temp[1]*d3[1]
              +temp[2]*d3[2]
              +temp[3]*d3[3]);
}
/* Bicubic */
QImage
bc(const QImage src, const qreal s)
{
    const int w = src.width();
    const int h = src.height();
    const int w1 = w-1;
    const int h1 = h-1;
    const int nw = w*s;
    const int nh = h*s;

    QImage nimg(nw, nh, src.format());
    QRgb *nbits = (QRgb*)nimg.bits();
    const QRgb *bits = (QRgb*)src.bits();

    //struct timeval tv[2];
    //gettimeofday(tv, NULL);

    qreal d1[4];
    int dr[4][4], dg[4][4], db[4][4], da[4][4];
    qreal d3[4];

    for (int y = 0; y < nh; ++y)
    {
        const qreal y0 = y/s;
        const int yg = y0;

        const qreal y2 = y0-yg;
        d1[0] = bicubic_h(1+y2);
        d1[1] = bicubic_h(y2);
        d1[2] = bicubic_h(1-y2);
        d1[3] = bicubic_h(2-y2);

        for (int x = 0; x < nw; ++x)
        {
            const qreal x0 = x/s;
            const int xg = x0;

            const qreal x2 = x0-xg;
            d3[0] = bicubic_h(1+x2);
            d3[1] = bicubic_h(x2);
            d3[2] = bicubic_h(1-x2);
            d3[3] = bicubic_h(2-x2);

            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    const QRgb rgba(*(bits
                                      +(std::min(std::max(xg+j-1, 0), w1))
                                      +(std::min(std::max(yg+i-1, 0), h1)*w)));
                    dr[i][j] = qRed(rgba);
                    dg[i][j] = qGreen(rgba);
                    db[i][j] = qBlue(rgba);
                    da[i][j] = qAlpha(rgba);
                }
            }

            *(nbits+x) = qRgba(
                        std::min(std::max(bicubic_matmul(d1, dr, d3), 0), 0xFF),
                        std::min(std::max(bicubic_matmul(d1, dg, d3), 0), 0xFF),
                        std::min(std::max(bicubic_matmul(d1, db, d3), 0), 0xFF),
                        std::min(std::max(bicubic_matmul(d1, da, d3), 0), 0xFF));
        }
        nbits += nw;
    }
    //gettimeofday(tv+1, NULL);
    //fprintf(stderr, "bicubic: %f\n", elapsed_time(tv));

    return nimg;
}

