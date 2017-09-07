/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#ifndef YUVPIXEL_H
#define YUVPIXEL_H

#include <QObject>
#include <QColor>
#include <QDebug>
#include <QSharedPointer>

class YuvPixel
{
public:
    YuvPixel(unsigned int x, unsigned int y);

    struct Coordinate {
        unsigned int x;
        unsigned int y;
    }coordinate;

    struct Color {
        quint8 luminance,chromaU,chromaV;
        quint8 r,g,b;
    }color;

    Color luminance;
    Color chromaU;
    Color chromaV;

    Color mseCombined;
    Color mseLuminance;
    Color mseChromaU;
    Color mseChromaV;

    struct MeanSquareError {
        struct Values {
            int luminance=0;
            int chromaU=0;
            int chromaV=0;
            int sum=0;
        }raw;

        Values normalized;

        QSharedPointer<int> sumMax;
        QSharedPointer<int> luminanceMax;
        QSharedPointer<int> chromaUMax;
        QSharedPointer<int> chromaVMax;
    }mse;

    void update(void);

    //QColor color;
};

#endif // YUVPIXEL_H
