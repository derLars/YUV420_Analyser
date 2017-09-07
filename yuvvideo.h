/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#ifndef YUVVIDEO_H
#define YUVVIDEO_H

#include <QObject>

#include <QList>
#include <QThread>

#include <QMutex>
#include <QWaitCondition>

#include "yuvframe.h"

class YuvVideo : public QThread
{
    Q_OBJECT
public:   
    YuvVideo(QByteArray bytes, const QString name, const unsigned int width, const unsigned int height , const unsigned int firstFrame, const unsigned int lastFrame, const unsigned int maxFrameNumber,QSharedPointer<YuvVideo> referenceVideo);
    YuvVideo(QByteArray bytes, const QString name, const unsigned int width, const unsigned int height , const unsigned int firstFrame, const unsigned int lastFrame, const unsigned int maxFrameNumber);

    QSharedPointer<YuvFrame> getFrame(int index);

    void calculatePeakSignalToNoiseRatio(QSharedPointer<YuvVideo> originalVideo);

    bool frameValid(const int frameNumber);

    void run();

    QString name;

    QByteArray bytes;

    unsigned int width;
    unsigned int height;

    unsigned int firstFrame;
    unsigned int lastFrame;

    unsigned int maxFrameNumber;

    bool valid;

    QSharedPointer<YuvVideo> referenceVideo;

    QList<QSharedPointer<YuvFrame> > frames;

    YuvFrame::Frame getVideoFrame(unsigned int index, int frameType);
    YuvFrame::Frame getVideoFrame(unsigned int index, int frameType, QList<QSharedPointer<YuvVideo> > processedVideos);
private:

    QMutex frameMutex;
    QWaitCondition frameAvailable;
signals:
    //void videoLoaded(void);
    void frameLoaded(int index);
};

#endif // YUVVIDEO_H
