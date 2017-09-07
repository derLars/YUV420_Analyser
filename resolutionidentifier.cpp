/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#include "resolutionidentifier.h"
#include "ui_resolutionidentifier.h"

#include <QMessageBox>
#include <QDebug>

ResolutionIdentifier::ResolutionIdentifier(QWidget *parent) :
    QDialog(parent), ui(new Ui::ResolutionIdentifier) {
    ui->setupUi(this);

    width.append(128); height.append(96);
    width.append(176); height.append(144);
    width.append(256); height.append(192);
    width.append(352); height.append(240);
    width.append(352); height.append(288);
    width.append(512); height.append(384);
    width.append(704); height.append(480);
    width.append(704); height.append(576);
    width.append(1408);height.append(1152);
    width.append(528); height.append(382);
    width.append(800); height.append(600);
    width.append(1920);height.append(1080);

    index = 0;
}

void ResolutionIdentifier::identifyResolution(QByteArray videoBytes) {
    availableFrames = 0;
    this->bytes = videoBytes;

    if(videoBytes.size() > 0 && isPossible(width.at(index),height.at(index),videoBytes.size())) {       
        video = QSharedPointer<YuvVideo>::create(videoBytes,"",width.at(index),height.at(index),1,4,4);

        connect(video.data(),SIGNAL(frameLoaded(int)),this,SLOT(frameLoaded(int)));
        video->start();

    }else if(videoBytes.size() > 0 && index < width.size()-1) {
        index++;
        identifyResolution(videoBytes);
    }else {
        index = 0;
        emit resolutionNotIdentifiable();
    }
}

void ResolutionIdentifier::frameLoaded(int frameNumber) {
    if(frameNumber == 3) {
        auto scene1 = QSharedPointer<QGraphicsScene>::create();
        auto scene2 = QSharedPointer<QGraphicsScene>::create();
        auto scene3 = QSharedPointer<QGraphicsScene>::create();
        auto scene4 = QSharedPointer<QGraphicsScene>::create();

        scene1->addPixmap(QPixmap::fromImage((*(video->getVideoFrame(0,0).image.data())).scaled(100,100)));
        scene2->addPixmap(QPixmap::fromImage((*(video->getVideoFrame(1,0).image.data())).scaled(100,100)));
        scene3->addPixmap(QPixmap::fromImage((*(video->getVideoFrame(2,0).image.data())).scaled(100,100)));
        scene4->addPixmap(QPixmap::fromImage((*(video->getVideoFrame(3,0).image.data())).scaled(100,100)));

        ui->graphicsView->setScene(scene1.data());
        ui->graphicsView_2->setScene(scene2.data());
        ui->graphicsView_3->setScene(scene3.data());
        ui->graphicsView_4->setScene(scene4.data());

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "identify", "Can you see good frames?",
                                       QMessageBox::Yes|QMessageBox::No);
         if (reply == QMessageBox::Yes) {
            emit resolutionIdentified(width.at(index), height.at(index));
            index = 0;
         } else {
            index++;
            identifyResolution(bytes);
         }
    }
}

bool ResolutionIdentifier::isPossible(int width, int height, int numOfBytes) {
    return ((numOfBytes % ((width * height * 3)/2)) == 0);
}

ResolutionIdentifier::~ResolutionIdentifier() {
    delete ui;
}
