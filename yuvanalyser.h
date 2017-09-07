/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#ifndef YUVANALYSER_H
#define YUVANALYSER_H

#include <QObject>

#include "yuvvideo.h"
class YuvAnalyser : public QObject
{
    Q_OBJECT
public:
    YuvAnalyser();

private:
    QSharedPointer<YuvVideo> referenceVideo;

    QList<QSharedPointer<YuvVideo> > processedVideos;

    unsigned int width;
    unsigned int height;
    unsigned int startFrame;
    unsigned int lastFrame;
    unsigned int maxFrameNumber;

    unsigned int loadedFrame;

public slots: 
    void startAnalysis(QByteArray referenceVideo, QString referenceName, QList<QByteArray> processedVideos, QList<QString> processedNames, int width,int height, int startFrame, int lastFrame);

    void frameLoaded(int frameNumber);
signals:
    void displayVideo(QSharedPointer<YuvVideo> referenceVideo, QList<QSharedPointer<YuvVideo> > processedVideos, unsigned int startFrame, unsigned int lastFrame);

    void framesAvailable(unsigned int);

};

#endif // YUVANALYSER_H
