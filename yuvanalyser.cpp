/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#include "yuvanalyser.h"

#include <QDebug>

YuvAnalyser::YuvAnalyser()
{

}

void YuvAnalyser::startAnalysis(QByteArray referenceVideo, QString referenceName, QList<QByteArray> processedVideos, QList<QString> processedNames, int width,int height, int startFrame, int lastFrame) {
    this->width      = width;
    this->height     = height;
    this->startFrame = startFrame;
    this->lastFrame  = lastFrame;
    this->maxFrameNumber = (referenceVideo.size() * 2)/(width*height*3);
    //this->maxFrameNumber = lastFrame - startFrame + 1;

    this->referenceVideo = QSharedPointer<YuvVideo>::create(referenceVideo,referenceName,width,height,startFrame,lastFrame,maxFrameNumber);
    connect(this->referenceVideo.data(),SIGNAL(frameLoaded(int)), this,SLOT(frameLoaded(int)));
    this->referenceVideo->start();

    for(int i=0;i<processedVideos.size(); i++) {
        this->processedVideos.append(QSharedPointer<YuvVideo>::create(processedVideos.at(i),processedNames.at(i), width, height, startFrame, lastFrame, maxFrameNumber,this->referenceVideo));
        connect(this->processedVideos.last().data(),SIGNAL(frameLoaded(int)), this,SLOT(frameLoaded(int)));
        this->processedVideos.last()->start();
    }

    qDebug() << "reference name: " << referenceName;
    qDebug() << "reference video size: " << referenceVideo.size();
    qDebug() << "Resolution: " << width << "x" << height;
    qDebug() << "From frame " << startFrame << " to " << lastFrame;
    qDebug() << "---------------------";
}

void YuvAnalyser::frameLoaded(int frameNumber) {
    if(referenceVideo->frameValid(frameNumber)){
        bool allFramesValid = true;
        for(auto processedVideo : processedVideos) {
            allFramesValid = allFramesValid ? processedVideo->frameValid(frameNumber) : allFramesValid;
        }

        if(allFramesValid) {           
            emit framesAvailable(frameNumber);
            if(frameNumber == 0) {
                emit displayVideo(referenceVideo,processedVideos,startFrame,lastFrame);
            }
        }        
    }
}

