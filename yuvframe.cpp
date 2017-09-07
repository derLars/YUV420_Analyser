/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#include "yuvframe.h"

#include <math.h>
#include <yuvvideo.h>

YuvFrame::YuvFrame(unsigned int width, unsigned int height, QByteArray luminance, QByteArray chromaU, QByteArray chromaV, unsigned int frameNumber, unsigned int frameInVideo)
    : width(width), height(height), luminance(luminance),chromaU(chromaU),chromaV(chromaV),frameNumber(frameNumber),frameInVideo(frameInVideo) {

    frameValid = true;
    defineColors();
}

YuvFrame::YuvFrame(QSharedPointer<YuvFrame> prevFrame, unsigned int width, unsigned int height, QByteArray luminance, QByteArray chromaU, QByteArray chromaV, unsigned int frameNumber, unsigned int frameInVideo)
    : width(width), height(height), luminance(luminance),chromaU(chromaU),chromaV(chromaV),frameNumber(frameNumber),prevFrame(prevFrame),frameInVideo(frameInVideo) {

    frameValid = true;
    defineColors();
}

YuvFrame::YuvFrame(unsigned int width, unsigned int height, QByteArray luminance, QByteArray chromaU, QByteArray chromaV, unsigned int frameNumber, unsigned int frameInVideo, QSharedPointer<YuvFrame> referenceFrame)
    : width(width), height(height), luminance(luminance),chromaU(chromaU),chromaV(chromaV),frameNumber(frameNumber),frameInVideo(frameInVideo),referenceFrame(referenceFrame) {

    calculatePeakSignalToNoiseRatio(referenceFrame);

    frameValid = true;
    defineColors();
}

YuvFrame::YuvFrame(QSharedPointer<YuvFrame> prevFrame, unsigned int width, unsigned int height, QByteArray luminance, QByteArray chromaU, QByteArray chromaV, unsigned int frameNumber, unsigned int frameInVideo, QSharedPointer<YuvFrame> referenceFrame)
    : width(width), height(height), luminance(luminance),chromaU(chromaU),chromaV(chromaV),frameNumber(frameNumber),frameInVideo(frameInVideo),referenceFrame(referenceFrame),prevFrame(prevFrame) {

    calculatePeakSignalToNoiseRatio(referenceFrame);

    frameValid = true;
    defineColors();
}

void YuvFrame::defineColors(void) {
    //red
    intraFrameColors.append(qRgb(205,38,38));
    intraFrameColors.append(qRgb(238,44,44));
    intraFrameColors.append(qRgb(255,48,48));
    intraFrameColors.append(qRgb(255,64,64));
    intraFrameColors.append(qRgb(255,106,106));

    //orange
    intraFrameColors.append(qRgb(255,127,0));
    intraFrameColors.append(qRgb(255,153,18));
    intraFrameColors.append(qRgb(255,165,0));
    intraFrameColors.append(qRgb(255,165,79));
    intraFrameColors.append(qRgb(255,193,37));

    //yellow
    intraFrameColors.append(qRgb(255,215,0));
    intraFrameColors.append(qRgb(238,238,0));
    intraFrameColors.append(qRgb(255,255,0));
    intraFrameColors.append(qRgb(255,246,143));
    intraFrameColors.append(qRgb(255,250,205));

    //green
    intraFrameColors.append(qRgb(0,100,0));
    intraFrameColors.append(qRgb(0,139,0));
    intraFrameColors.append(qRgb(50,205,50));
    intraFrameColors.append(qRgb(0,255,0));
    intraFrameColors.append(qRgb(152,251,152));

    //blue
    intraFrameColors.append(qRgb(0,0,128));
    intraFrameColors.append(qRgb(0,0,205));
    intraFrameColors.append(qRgb(0,0,255));
    intraFrameColors.append(qRgb(58,95,205));
    intraFrameColors.append(qRgb(100,149,237));

    //purple
    intraFrameColors.append(qRgb(122,55,139));
    intraFrameColors.append(qRgb(148,0,211));
    intraFrameColors.append(qRgb(191,62,255));
    intraFrameColors.append(qRgb(209,95,238));
    intraFrameColors.append(qRgb(224,102,255));

    //pink
    intraFrameColors.append(qRgb(205,16,118));
    intraFrameColors.append(qRgb(255,20,147));
    intraFrameColors.append(qRgb(255,110,180));
    intraFrameColors.append(qRgb(255,130,171));
    intraFrameColors.append(qRgb(255,174,185));

    //grey
    intraFrameColors.append(qRgb(110,110,110));
    intraFrameColors.append(qRgb(140,140,140));
    intraFrameColors.append(qRgb(170,170,170));
    intraFrameColors.append(qRgb(200,200,200));
    intraFrameColors.append(qRgb(230,230,230));
}

YuvFrame::Frame YuvFrame::getFrame(unsigned const int frameType) {
    switch(frameType) {
    case 0:
        return getFullFrame();
    case 1:
        return getLuminanceFrame();
    case 2:
        return getChromaUFrame();
    case 3:
        return getChromaVFrame();
    case 4:
        return getInterFrameLuminanceFrame();
    case 5:
        return getInterFrameChromaUFrame();
    case 6:
        return getInterFrameChromaVFrame();
    case 7:
        return getIntraFrameLuminanceFrame();
    case 8:
        return getIntraFrameChromaUFrame();
    case 9:
        return getIntraFrameChromaVFrame();
    default:
        return getFullFrame();
        break;
    }
}

YuvFrame::Frame YuvFrame::getFrame(unsigned const int frameType, QList<QSharedPointer<YuvVideo> > processedVideos) {
    switch(frameType) {
    case 0:
        return getFullFrame();
    case 1:
        return getLuminanceFrame();
    case 2:
        return getChromaUFrame();
    case 3:
        return getChromaVFrame();
    case 4:
        return getInterFrameLuminanceFrame();
    case 5:
        return getInterFrameChromaUFrame();
    case 6:
        return getInterFrameChromaVFrame();
    case 7:
        return getIntraFrameLuminanceFrame();
    case 8:
        return getIntraFrameChromaUFrame();
    case 9:
        return getIntraFrameChromaVFrame();
    case 10:
        return getMseLuminanceFrame(processedVideos);
    case 11:
        return getMseChromaUFrame(processedVideos);
    case 12:
        return getMseChromaVFrame(processedVideos);
    default:
        return getFullFrame();
        break;
    }
}

YuvFrame::Frame YuvFrame::getFullFrame(void) {
    auto thisImage = QSharedPointer<QImage>::create(width,height,QImage::Format_RGB888);

    int k=0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
        int idx1 = (i*2)+(k*width);
        int idx2 = (i*2)+((k+1)*width);
        int idx3 = idx1 +1;
        int idx4 = idx2 +1;

        if(i!=0 && i%(width/2)==0) {
            k++;
        }

        int idx[4] = {idx1,idx2,idx3,idx4};
        for(int j=0; j<4; j++) {
            quint8 pix[3];

            auto val1 = (quint8)luminance[idx[j]] + (1.370705 * ((quint8)chromaV[i]-128));
            auto val2 = (quint8)luminance[idx[j]] - (0.698001 * ((quint8)chromaV[i]-128)) - (0.337633 * ((quint8)chromaU[i]-128));
            auto val3 = (quint8)luminance[idx[j]] + (1.732446 * ((quint8)chromaU[i]-128));

            pix[0] = val1 > 0 ? val1 : 0;
            pix[1] = val2 > 0 ? val2 : 0;
            pix[2] = val3 > 0 ? val3 : 0;

            pix[0] = val1 < 255 ? pix[0] : 255;
            pix[1] = val2 < 255 ? pix[1] : 255;
            pix[2] = val3 < 255 ? pix[2] : 255;

            thisImage->setPixel(idx[j]%width,idx[j]/width,qRgb(pix[0],pix[1],pix[2]));
        }
    }

    frame.image = thisImage;
    return frame;
}

YuvFrame::Frame YuvFrame::getLuminanceFrame(void) {
    auto thisImage = QSharedPointer<QImage>::create(width,height,QImage::Format_RGB888);

    int k=0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
        int idx1 = (i*2)+(k*width);
        int idx2 = (i*2)+((k+1)*width);
        int idx3 = idx1 +1;
        int idx4 = idx2 +1;

        if(i!=0 && i%(width/2)==0) {
            k++;
        }

        int idx[4] = {idx1,idx2,idx3,idx4};
        for(int j=0; j<4; j++) {
            quint8 pix[3];

            pix[0] = (quint8)luminance[idx[j]];
            pix[1] = (quint8)luminance[idx[j]];
            pix[2] = (quint8)luminance[idx[j]];


            thisImage->setPixel(idx[j]%width,idx[j]/width,qRgb(pix[0],pix[1],pix[2]));
        }
    }

    frame.image = thisImage;
    return frame;
}

YuvFrame::Frame YuvFrame::getChromaUFrame(void) {
    auto thisImage = QSharedPointer<QImage>::create(width,height,QImage::Format_RGB888);

    int k=0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
        int idx1 = (i*2)+(k*width);
        int idx2 = (i*2)+((k+1)*width);
        int idx3 = idx1 +1;
        int idx4 = idx2 +1;

        if(i!=0 && i%(width/2)==0) {
            k++;
        }

        int idx[4] = {idx1,idx2,idx3,idx4};
        for(int j=0; j<4; j++) {
            quint8 pix[3];

            pix[0] = 0;
            pix[1] = 255 - chromaU[i];
            pix[2] = chromaU[i];

            thisImage->setPixel(idx[j]%width,idx[j]/width,qRgb(pix[0],pix[1],pix[2]));
        }
    }

    frame.image = thisImage;
    return frame;
}

YuvFrame::Frame YuvFrame::getChromaVFrame(void) {
    auto thisImage = QSharedPointer<QImage>::create(width,height,QImage::Format_RGB888);

    int k=0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
        int idx1 = (i*2)+(k*width);
        int idx2 = (i*2)+((k+1)*width);
        int idx3 = idx1 +1;
        int idx4 = idx2 +1;

        if(i!=0 && i%(width/2)==0) {
            k++;
        }

        int idx[4] = {idx1,idx2,idx3,idx4};
        for(int j=0; j<4; j++) {
            quint8 pix[3];

            pix[0] = chromaV[i];
            pix[1] = 255 - chromaV[i];
            pix[2] = 0;

            thisImage->setPixel(idx[j]%width,idx[j]/width,qRgb(pix[0],pix[1],pix[2]));
        }
    }

    frame.image = thisImage;
    return frame;
}

YuvFrame::Frame YuvFrame::getMseLuminanceFrame(QList<QSharedPointer<YuvVideo> > processedVideos) {
    auto thisImage = QSharedPointer<QImage>::create(width,height,QImage::Format_RGB888);

    unsigned int maxLuminanceMSE = 0;
    for(auto video : processedVideos) {
        auto otherFrameMaxLuminanceMse = video->getFrame(frameNumber)->getMaxLuminanceMse();
        maxLuminanceMSE = maxLuminanceMSE > otherFrameMaxLuminanceMse ? maxLuminanceMSE : otherFrameMaxLuminanceMse;
    }

    int k=0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
        int idx1 = (i*2)+(k*width);
        int idx2 = (i*2)+((k+1)*width);
        int idx3 = idx1 +1;
        int idx4 = idx2 +1;

        if(i!=0 && i%(width/2)==0) {
            k++;
        }

        int idx[4] = {idx1,idx2,idx3,idx4};
        for(int j=0; j<4; j++) {
            quint8 pix[3];

            quint8 lumValue = (quint8)luminance.at(idx[j]);
            quint8 refLumValue = (quint8)referenceFrame->luminance.at(idx[j]);

            auto luminanceMSE = (((refLumValue - lumValue) * (refLumValue - lumValue)) * 255)/maxLuminanceMSE;

            pix[0] = 255 - luminanceMSE;
            pix[1] = 255 - luminanceMSE;
            pix[2] = 255 - luminanceMSE;

            thisImage->setPixel(idx[j]%width,idx[j]/width,qRgb(pix[0],pix[1],pix[2]));
        }
    }

    frame.image = thisImage;
    return frame;
}

YuvFrame::Frame YuvFrame::getMseChromaUFrame(QList<QSharedPointer<YuvVideo> > processedVideos) {
    auto thisImage = QSharedPointer<QImage>::create(width,height,QImage::Format_RGB888);

    unsigned int maxChromaUMSE = 0;
    for(auto video : processedVideos) {
        auto otherFrameMaxChromaUMse = video->getFrame(frameNumber)->getMaxChromaUMse();
        maxChromaUMSE = maxChromaUMSE > otherFrameMaxChromaUMse ? maxChromaUMSE : otherFrameMaxChromaUMse;
    }

    int k=0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
        int idx1 = (i*2)+(k*width);
        int idx2 = (i*2)+((k+1)*width);
        int idx3 = idx1 +1;
        int idx4 = idx2 +1;

        if(i!=0 && i%(width/2)==0) {
            k++;
        }

        int idx[4] = {idx1,idx2,idx3,idx4};
        for(int j=0; j<4; j++) {
            quint8 pix[3];

            quint8 chromaUValue = (quint8)chromaU.at(i);
            quint8 refChromaUValue = (quint8)referenceFrame->chromaU.at(i);

            auto chromaUMSE = (((refChromaUValue - chromaUValue) * (refChromaUValue - chromaUValue)) * 255)/maxChromaUMSE;
            //auto luminanceMSE = (((refLumValue - lumValue) * (refLumValue - lumValue)))/255;

            pix[0] = 255 - chromaUMSE;
            pix[1] = 255 - chromaUMSE;
            pix[2] = 255 - chromaUMSE;

            thisImage->setPixel(idx[j]%width,idx[j]/width,qRgb(pix[0],pix[1],pix[2]));
        }
    }

    frame.image = thisImage;
    return frame;
}

YuvFrame::Frame YuvFrame::getMseChromaVFrame(QList<QSharedPointer<YuvVideo> > processedVideos) {
    auto thisImage = QSharedPointer<QImage>::create(width,height,QImage::Format_RGB888);

    unsigned int maxChromaVMSE = 0;
    for(auto video : processedVideos) {
        auto otherFrameMaxChromaVMse = video->getFrame(frameNumber)->getMaxChromaVMse();
        maxChromaVMSE = maxChromaVMSE > otherFrameMaxChromaVMse ? maxChromaVMSE : otherFrameMaxChromaVMse;
    }

    int k=0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
        int idx1 = (i*2)+(k*width);
        int idx2 = (i*2)+((k+1)*width);
        int idx3 = idx1 +1;
        int idx4 = idx2 +1;

        if(i!=0 && i%(width/2)==0) {
            k++;
        }

        int idx[4] = {idx1,idx2,idx3,idx4};
        for(int j=0; j<4; j++) {
            quint8 pix[3];

            quint8 ChromaVValue = (quint8)chromaV.at(i);
            quint8 refChromaVValue = (quint8)referenceFrame->chromaV.at(i);

            auto ChromaVMSE = (((refChromaVValue - ChromaVValue) * (refChromaVValue - ChromaVValue)) * 255)/maxChromaVMSE;
            //auto luminanceMSE = (((refLumValue - lumValue) * (refLumValue - lumValue)))/255;

            pix[0] = 255 - ChromaVMSE;
            pix[1] = 255 - ChromaVMSE;
            pix[2] = 255 - ChromaVMSE;

            thisImage->setPixel(idx[j]%width,idx[j]/width,qRgb(pix[0],pix[1],pix[2]));
        }
    }

    frame.image = thisImage;
    return frame;
}

YuvFrame::Frame YuvFrame::getInterFrameLuminanceFrame(void) {
    auto thisImage = QSharedPointer<QImage>::create(width,height,QImage::Format_RGB888);

    if(frameNumber == 0) {
        return getLuminanceFrame();
    }

    int k=0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
        int idx1 = (i*2)+(k*width);
        int idx2 = (i*2)+((k+1)*width);
        int idx3 = idx1 +1;
        int idx4 = idx2 +1;

        if(i!=0 && i%(width/2)==0) {
            k++;
        }

        int idx[4] = {idx1,idx2,idx3,idx4};
        for(int j=0; j<4; j++) {
             quint8 pix[3];

            auto difference = (quint8)luminance[idx[j]] - (quint8)prevFrame->luminance[idx[j]];
            //qDebug() << "Difference: " << difference << " with: " << (quint8)luminance[idx[j]] << " - " << (quint8)prevFrame->luminance[idx[j]];
            if(difference > 1 || difference < -1) {
                pix[0] = (quint8)luminance[idx[j]];
                pix[1] = (quint8)luminance[idx[j]];
                pix[2] = (quint8)luminance[idx[j]];
            }else {
                pix[0] = 255;
                pix[1] = (quint8)luminance[idx[j]];
                pix[2] = (quint8)luminance[idx[j]];
            }

            thisImage->setPixel(idx[j]%width,idx[j]/width,qRgb(pix[0],pix[1],pix[2]));
        }
    }

    frame.image = thisImage;
    return frame;
}

YuvFrame::Frame YuvFrame::getInterFrameChromaUFrame(void) {
    auto thisImage = QSharedPointer<QImage>::create(width,height,QImage::Format_RGB888);

    if(frameNumber == 0) {
        return getLuminanceFrame();
    }

    int k=0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
        int idx1 = (i*2)+(k*width);
        int idx2 = (i*2)+((k+1)*width);
        int idx3 = idx1 +1;
        int idx4 = idx2 +1;

        if(i!=0 && i%(width/2)==0) {
            k++;
        }

        int idx[4] = {idx1,idx2,idx3,idx4};
        for(int j=0; j<4; j++) {
            quint8 pix[3];

           auto difference = (quint8)chromaU[i] - (quint8)prevFrame->chromaU[i];
           //qDebug() << "Difference: " << difference << " with: " << (quint8)luminance[idx[j]] << " - " << (quint8)prevFrame->luminance[idx[j]];
           if(difference > 1 || difference < -1) {
               pix[0] = (quint8)luminance[idx[j]];
               pix[1] = (quint8)luminance[idx[j]];
               pix[2] = (quint8)luminance[idx[j]];
           }else {
               pix[0] = 255;
               pix[1] = (quint8)luminance[idx[j]];
               pix[2] = (quint8)luminance[idx[j]];
           }

            thisImage->setPixel(idx[j]%width,idx[j]/width,qRgb(pix[0],pix[1],pix[2]));
        }
    }

    frame.image = thisImage;
    return frame;
}

YuvFrame::Frame YuvFrame::getInterFrameChromaVFrame(void) {
    auto thisImage = QSharedPointer<QImage>::create(width,height,QImage::Format_RGB888);

    if(frameNumber == 0) {
        return getLuminanceFrame();
    }

    int k=0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
        int idx1 = (i*2)+(k*width);
        int idx2 = (i*2)+((k+1)*width);
        int idx3 = idx1 +1;
        int idx4 = idx2 +1;

        if(i!=0 && i%(width/2)==0) {
            k++;
        }

        int idx[4] = {idx1,idx2,idx3,idx4};
        for(int j=0; j<4; j++) {
            quint8 pix[3];

           auto difference = (quint8)chromaV[i] - (quint8)prevFrame->chromaV[i];
           //qDebug() << "Difference: " << difference << " with: " << (quint8)luminance[idx[j]] << " - " << (quint8)prevFrame->luminance[idx[j]];
           if(difference > 1 || difference < -1) {
               pix[0] = (quint8)luminance[idx[j]];
               pix[1] = (quint8)luminance[idx[j]];
               pix[2] = (quint8)luminance[idx[j]];
           }else {
               pix[0] = 255;
               pix[1] = (quint8)luminance[idx[j]];
               pix[2] = (quint8)luminance[idx[j]];
           }

            thisImage->setPixel(idx[j]%width,idx[j]/width,qRgb(pix[0],pix[1],pix[2]));
        }
    }

    frame.image = thisImage;
    return frame;
}

YuvFrame::Frame YuvFrame::getIntraFrameLuminanceFrame(void) {
    auto thisImage = QSharedPointer<QImage>::create(width,height,QImage::Format_RGB888);

    int k=0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
        int idx1 = (i*2)+(k*width);
        int idx2 = (i*2)+((k+1)*width);
        int idx3 = idx1 +1;
        int idx4 = idx2 +1;

        if(i!=0 && i%(width/2)==0) {
            k++;
        }

        int idx[4] = {idx1,idx2,idx3,idx4};
        for(int j=0; j<4; j++) {
            thisImage->setPixel(idx[j]%width,idx[j]/width,intraFrameColors.at(int((quint8)luminance[idx[j]]/6.4)));
            //thisImage->setPixel(idx[j]%width,idx[j]/width,intraFrameColors.at(int((quint8)luminance[idx[j]]/12.75)));
        }
    }

    frame.image = thisImage;
    return frame;
}

YuvFrame::Frame YuvFrame::getIntraFrameChromaUFrame(void) {
    auto thisImage = QSharedPointer<QImage>::create(width,height,QImage::Format_RGB888);

    int k=0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
        int idx1 = (i*2)+(k*width);
        int idx2 = (i*2)+((k+1)*width);
        int idx3 = idx1 +1;
        int idx4 = idx2 +1;

        if(i!=0 && i%(width/2)==0) {
            k++;
        }

        int idx[4] = {idx1,idx2,idx3,idx4};
        for(int j=0; j<4; j++) {
            thisImage->setPixel(idx[j]%width,idx[j]/width,intraFrameColors.at(int((quint8)chromaU[i]/6.4)));
            //thisImage->setPixel(idx[j]%width,idx[j]/width,intraFrameColors.at(int((quint8)chromaU[i]/12.75)));
        }
    }

    frame.image = thisImage;
    return frame;
}

YuvFrame::Frame YuvFrame::getIntraFrameChromaVFrame(void) {
    auto thisImage = QSharedPointer<QImage>::create(width,height,QImage::Format_RGB888);

    int k=0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
        int idx1 = (i*2)+(k*width);
        int idx2 = (i*2)+((k+1)*width);
        int idx3 = idx1 +1;
        int idx4 = idx2 +1;

        if(i!=0 && i%(width/2)==0) {
            k++;
        }

        int idx[4] = {idx1,idx2,idx3,idx4};
        for(int j=0; j<4; j++) {
            thisImage->setPixel(idx[j]%width,idx[j]/width,intraFrameColors.at(int((quint8)chromaV[i]/6.4)));
            //thisImage->setPixel(idx[j]%width,idx[j]/width,intraFrameColors.at(int((quint8)chromaV[i]/12.75)));
        }
    }

    frame.image = thisImage;
    return frame;
}

unsigned int YuvFrame::getMaxLuminanceMse(void) {
    int k=0;
    int maxLuminanceMSE = 0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
        int idx1 = (i*2)+(k*width);
        int idx2 = (i*2)+((k+1)*width);
        int idx3 = idx1 +1;
        int idx4 = idx2 +1;

        if(i!=0 && i%(width/2)==0) {
            k++;
        }

        int idx[4] = {idx1,idx2,idx3,idx4};
        for(int j=0; j<4; j++) {
            quint8 lumValue = (quint8)luminance.at(idx[j]);
            quint8 refLumValue = (quint8)referenceFrame->luminance.at(idx[j]);

            auto luminanceMSE = ((refLumValue - lumValue) * (refLumValue - lumValue));
            maxLuminanceMSE = maxLuminanceMSE > luminanceMSE ? maxLuminanceMSE : luminanceMSE;
        }
    }
    return maxLuminanceMSE;
}

unsigned int YuvFrame::getMaxChromaUMse(void) {
    int maxChromaUMSE = 0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
            quint8 chromaUValue = (quint8)chromaU.at(i);
            quint8 refcChromaUValue = (quint8)referenceFrame->chromaU.at(i);

            auto chromaUMSE = ((refcChromaUValue - chromaUValue) * (refcChromaUValue - chromaUValue));
            maxChromaUMSE = maxChromaUMSE > chromaUMSE ? maxChromaUMSE : chromaUMSE;
    }
    return maxChromaUMSE;
}

unsigned int YuvFrame::getMaxChromaVMse(void) {
    unsigned int maxChromaVMSE = 0;
    for(unsigned int i=0; i<(width*height)/4;i++) {
            quint8 chromaVValue = (quint8)chromaV.at(i);
            quint8 refcChromaVValue = (quint8)referenceFrame->chromaV.at(i);

            maxChromaVMSE = refcChromaVValue > chromaVValue ? refcChromaVValue : chromaVValue;
    }
    return maxChromaVMSE;
}

void YuvFrame::calculatePeakSignalToNoiseRatio(QSharedPointer<YuvFrame> originalFrame) {
    double mse = 0;
    double msePixel;

    int i;
    for(i=0; i<luminance.size(); i++) {
        msePixel  = (originalFrame->luminance.at(i) - this->luminance.at(i)) * (originalFrame->luminance.at(i) - this->luminance.at(i));
        mse += msePixel;
        //qDebug() << "msePixel: " << msePixel;
    }

    frame.mse = mse/i;
    frame.psnr = (20 * log10(pow (2, 8))) - (10 * log10(frame.mse));

    //qDebug() << "for " << frameNumber << ": mse: " << frame.mse << " psnr: " << frame.psnr;

}

void YuvFrame::convertRGBToHSL(const quint8 r, const quint8 g, const quint8 b, double& h, double& s, double& l) {
    double rRef = r/255.0;
    double gRef = g/255.0;
    double bRef = b/255.0;

    double min = rRef;
    min = min < gRef ? min : gRef;
    min = min < bRef ? min : bRef;

    double max = rRef;
    max = max > gRef ? max : gRef;
    max = max > bRef ? max : bRef;

    //calculate l
    l = (min + max)/2;

    if(r == g && g == b) {
        h = 0;
        s = 0;
    }else {
        //calculate s
        if(l >= 0.5) {
            s = (max - min)/(2.0-max-min);
        } else {
            s = (max-min)/(max+min);
        }

        //calculate h
        if(max == rRef) {
            h = 60 * (((gRef - bRef)/(max-min)));
            //h = (g-b)*60/(max-min);
        }else if(max == gRef) {
            h = 60 * (((bRef - rRef)/(max-min))+2.0);
            //h = (2.0 + (b-r))*60/(max-min);
        }else {
            h = 60 * (((rRef - gRef)/(max-min))+4.0);
            //h = (4.0 + (r-g))*60/(max-min);
        }
        h = h >=0 ? h : 360 - h;
    }

    //qDebug() << "r: " << r <<  " g: "<< g <<  " b: "<< b << " h: " << h << " s: " << s<<  " l: " << l;
}

double absolute(double val) {
    return val > 0 ? val : val * (-1);
}

void YuvFrame::convertHSLToRGB(quint8& r, quint8& g, quint8& b, const double h, const double s, const double l) {
    double absolC = absolute(2*l -1);
    double c = (1 - absolC) * s;

    double z = h/60;
    int zInt = h/60;
    z = z < 2 ? z : (zInt%2) + (z - zInt);

    double x = c * (1 - absolute(z - 1));

    double m = l - (c/2);

    if(h >= 0 && h < 60) {
        r = (c + m)*255;
        g = (x + m)*255;
        b = (0 + m)*255;
    }else if(h >= 60 && h < 120) {
        r = (x + m)*255;
        g = (c + m)*255;
        b = (0 + m)*255;
    }else if(h >= 120 && h < 180) {
        r = (0 + m)*255;
        g = (c + m)*255;
        b = (x + m)*255;
    }else if(h >= 180 && h < 240) {
        r = (0 + m)*255;
        g = (x + m)*255;
        b = (c + m)*255;
    }else if(h >= 240 && h < 300) {
        r = (x + m)*255;
        g = (0 + m)*255;
        b = (c + m)*255;
    }else {
        r = (c + m)*255;
        g = (0 + m)*255;
        b = (x + m)*255;
    }
}

