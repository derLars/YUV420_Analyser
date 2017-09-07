/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#include "yuvvideo.h"

#include <QFile>
#include <QDebug>

YuvVideo::YuvVideo(QByteArray bytes, const QString name, const unsigned int width, const unsigned int height , const unsigned int firstFrame, const unsigned int lastFrame, const unsigned int maxFrameNumber, QSharedPointer<YuvVideo> referenceVideo)
    :bytes(bytes),name(name),width(width),height(height),firstFrame(firstFrame),lastFrame(lastFrame),maxFrameNumber(maxFrameNumber),valid(false),referenceVideo(referenceVideo) {
}

YuvVideo::YuvVideo(QByteArray bytes, const QString name, const unsigned int width, const unsigned int height , const unsigned int firstFrame, const unsigned int lastFrame, const unsigned int maxFrameNumber)
    :bytes(bytes),name(name),width(width),height(height),firstFrame(firstFrame),lastFrame(lastFrame),maxFrameNumber(maxFrameNumber),valid(false) {

}

void YuvVideo::run(void) {
    unsigned int byteCounter = 0;
    unsigned int frameCounter = 0;
    unsigned int addedFrames = 0;

    while(byteCounter < bytes.size() && (frameCounter < lastFrame)) {
        QByteArray chroma;
        QByteArray u;
        QByteArray v;

        if(frameCounter >= firstFrame-1) {
            for(unsigned int i=0; i<width*height; i++) {
                chroma.append(bytes.at(byteCounter));
                byteCounter++;
            }

            for(unsigned int i=0; i<(width*height)/4; i++) {
                u.append(bytes.at(byteCounter));
                byteCounter++;
            }

            for(unsigned int i=0; i<(width*height)/4; i++) {
                v.append(bytes.at(byteCounter));
                byteCounter++;
            }

            frameMutex.lock();
            if(!referenceVideo.isNull()) {
                if(addedFrames == 0) {
                    frames.append(QSharedPointer<YuvFrame>::create(width,height,chroma,u,v,addedFrames,frameCounter, referenceVideo->getFrame(addedFrames)));
                }else{
                    frames.append(QSharedPointer<YuvFrame>::create(frames.last(),width,height,chroma,u,v,addedFrames,frameCounter, referenceVideo->getFrame(addedFrames)));
                }

            } else {
                 if(addedFrames == 0) {
                     frames.append(QSharedPointer<YuvFrame>::create(width,height,chroma,u,v,addedFrames,frameCounter));
                 } else {
                     frames.append(QSharedPointer<YuvFrame>::create(frames.last(),width,height,chroma,u,v,addedFrames,frameCounter));
                 }

            }
            frameMutex.unlock();
            frameAvailable.wakeAll();

            emit frameLoaded(addedFrames);

            addedFrames++;
        }else{
            byteCounter = byteCounter + (width*height) + (width*height)/2;
        }
        frameCounter++;
    }
    bytes.clear();
    valid = true;
}

bool YuvVideo::frameValid(const int frameNumber) {
    if(frames.size() > frameNumber) {
        auto result = frameNumber < frames.size() ? frames.at(frameNumber)->frameValid : false;
        return result;
    }
    return false;
}

QSharedPointer<YuvFrame> YuvVideo::getFrame(int index) {
    frameMutex.lock();
    if(frames.size() <= index) {
        frameAvailable.wait(&frameMutex);
    }
    frameMutex.unlock();
    return frames.at(index);
}

YuvFrame::Frame YuvVideo::getVideoFrame(unsigned int index, int frameType) {
    return frames.at(index)->getFrame(frameType);
}

YuvFrame::Frame YuvVideo::getVideoFrame(unsigned int index, int frameType,  QList<QSharedPointer<YuvVideo> > processedVideos) {
    return frames.at(index)->getFrame(frameType, processedVideos);
}

void YuvVideo::calculatePeakSignalToNoiseRatio(QSharedPointer<YuvVideo> originalVideo) {
    for(int i=0; i<frames.size(); i++) {
        frames.at(i)->calculatePeakSignalToNoiseRatio(originalVideo->getFrame(i));
    }
}
