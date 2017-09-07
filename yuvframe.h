/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#ifndef YUVFRAME_H
#define YUVFRAME_H

#include <QObject>
#include <QList>
#include <QSharedPointer>
#include <QGraphicsScene>

#include "yuvpixel.h"

class YuvVideo;

class YuvFrame
{
public:
    YuvFrame(unsigned int width, unsigned int height, QByteArray luminance, QByteArray chromaU, QByteArray chromaV, unsigned int frameNumber, unsigned int frameInVideo);
    YuvFrame(QSharedPointer<YuvFrame> prevFrame, unsigned int width, unsigned int height, QByteArray luminance, QByteArray chromaU, QByteArray chromaV, unsigned int frameNumber, unsigned int frameInVideo);

    YuvFrame(unsigned int width, unsigned int height, QByteArray luminance, QByteArray chromaU, QByteArray chromaV, unsigned int frameNumber, unsigned int frameInVideo, QSharedPointer<YuvFrame> referenceFrame);
    YuvFrame(QSharedPointer<YuvFrame> prevFrame, unsigned int width, unsigned int height, QByteArray luminance, QByteArray chromaU, QByteArray chromaV, unsigned int frameNumber, unsigned int frameInVideo, QSharedPointer<YuvFrame> referenceFrame);

    unsigned int width;
    unsigned int height;

    bool frameValid;

    unsigned int frameNumber;
    unsigned int frameInVideo;

    void calculatePeakSignalToNoiseRatio(QSharedPointer<YuvFrame> originalFrame);

    QList<QSharedPointer<YuvPixel> > pixels;

    struct Frame {
        QSharedPointer<QImage> image;
        double mse;
        double psnr;
    } frame;

    Frame getFullFrame(const unsigned int frameNumber);

    Frame getFrame(unsigned const int frameType, QList<QSharedPointer<YuvVideo> > processedVideos);
    Frame getFrame(unsigned const int frameType);
    Frame getFullFrame(void);
    Frame getLuminanceFrame(void);
    Frame getChromaUFrame(void);
    Frame getChromaVFrame(void);

    Frame getMseLuminanceFrame(QList<QSharedPointer<YuvVideo> > processedVideos);
    Frame getMseChromaUFrame(QList<QSharedPointer<YuvVideo> > processedVideos);
    Frame getMseChromaVFrame(QList<QSharedPointer<YuvVideo> > processedVideos);

    Frame getInterFrameLuminanceFrame(void);
    Frame getInterFrameChromaUFrame(void);
    Frame getInterFrameChromaVFrame(void);

    Frame getIntraFrameLuminanceFrame(void);
    Frame getIntraFrameChromaUFrame(void);
    Frame getIntraFrameChromaVFrame(void);

    unsigned int getMaxLuminanceMse(void);
    unsigned int getMaxChromaUMse(void);
    unsigned int getMaxChromaVMse(void);

    void convertRGBToHSL(const quint8 r, const quint8 g, const quint8 b, double &h, double &s, double &l);
    void convertHSLToRGB(quint8& r, quint8& g, quint8& b, const double h, const double s, const double l);

private:
    QByteArray luminance;
    QByteArray chromaU;
    QByteArray chromaV;

    QSharedPointer<YuvFrame> referenceFrame;
    QSharedPointer<YuvFrame> prevFrame;

    QList<QRgb> intraFrameColors;

    void defineColors(void);
};

#endif // YUVFRAME_H
