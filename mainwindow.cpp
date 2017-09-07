/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVideoFrame>
#include <QRgb>
#include <QDebug>
#include <QFileDialog>

#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(displayNewFrame(void)));
    connect(ui->zoomHorizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(displayNewFrame(void)));
    connect(ui->saveReferencePushButton,SIGNAL(clicked(bool)),this,SLOT(saveReferenceFrame()));
    connect(ui->saveProcessedPushButton,SIGNAL(clicked(bool)),this,SLOT(saveProcessedFrame()));   

    connect(ui->playPushButton,SIGNAL(clicked(bool)),this,SLOT(startAutoVideo()));
    connect(ui->stopPushButton,SIGNAL(clicked(bool)),this,SLOT(stopAutoVideo()));
    connect(ui->firstFramePushButton,SIGNAL(clicked(bool)),this,SLOT(firstFrame()));

    ui->videoViewReferenceGraphicsView->resize(818,618);
    ui->videoViewProcessedGraphicsView->resize(818,618);

    autoVideo = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayNewFrame(void) {
    unsigned int frameNumber = ui->horizontalSlider->value();
    int referenceFrameType = ui->referenceTypeComboBox->currentIndex();
    int processedFrameType = ui->processedTypeComboBox->currentIndex();
    int videoNumber = ui->processedVideoComboBox->currentIndex();

    double zoom = 1 + (ui->zoomHorizontalSlider->value() * 0.5);

    if(frameNumber > this->referenceVideo->maxFrameNumber) {
        ui->horizontalSlider->setValue(this->referenceVideo->maxFrameNumber);
        return;
    }

    referenceFrame = this->referenceVideo->getVideoFrame(frameNumber,referenceFrameType);
    processedFrame = this->processedVideo.at(videoNumber)->getVideoFrame(frameNumber,processedFrameType,processedVideo);

    referenceScene->addPixmap(QPixmap::fromImage(referenceFrame.image->scaled(800*zoom,600*zoom)));
    processedScene->addPixmap(QPixmap::fromImage(processedFrame.image->scaled(800*zoom,600*zoom)));

    ui->videoViewReferenceGraphicsView->setScene(referenceScene.data());
    ui->videoViewProcessedGraphicsView->setScene(processedScene.data());

    ui->videoViewReferenceGraphicsView->setSceneRect(0,0,800*zoom,600*zoom);
    ui->videoViewProcessedGraphicsView->setSceneRect(0,0,800*zoom,600*zoom);

    ui->nrFramesLabel->setText(QString::number(this->referenceVideo->getFrame(frameNumber)->frameInVideo + 1) + " of " + QString::number(this->referenceVideo->maxFrameNumber));

    ui->mseLabel->setText(QString::number(processedFrame.mse));
    ui->psnrLabel->setText(QString::number(processedFrame.psnr));
}

void MainWindow::displayVideo(QSharedPointer<YuvVideo> referenceVideo, QList<QSharedPointer<YuvVideo> > processedVideo,unsigned int startFrame, unsigned int lastFrame) {
    frames = lastFrame - startFrame +1;

    if(ui->horizontalSlider->value() > frames) {
        ui->horizontalSlider->setValue(frames);
    }
    ui->horizontalSlider->setMaximum(frames-1);


    if(ui->processedVideoComboBox->count() == 0) {
        for(auto video : processedVideo) {
            ui->processedVideoComboBox->addItem(video->name);
        }
    }

    ui->framesAvailableProgressBar->setMaximum(frames);

    ui->referenceTypeComboBox->setCurrentIndex(0);
    ui->processedTypeComboBox->setCurrentIndex(0);
    ui->processedVideoComboBox->setCurrentIndex(0);

    connect(ui->referenceTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(displayNewFrame(void)));
    connect(ui->processedTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(displayNewFrame(void)));
    connect(ui->processedVideoComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(displayNewFrame(void)));

    this->referenceVideo = referenceVideo;
    connect(referenceVideo.data(),SIGNAL(frameLoaded(int)),this,SLOT(frameLoaded(void)));

    this->processedVideo = processedVideo;

    referenceScene = QSharedPointer<QGraphicsScene>::create(this);
    processedScene = QSharedPointer<QGraphicsScene>::create(this);

    displayNewFrame();

    ui->horizontalSlider->setEnabled(frames > 1);

    show();
}

void MainWindow::frameLoaded(void) {
    if(ui->horizontalSlider->value() > frames) {
        ui->horizontalSlider->setValue(frames);
    }
    ui->horizontalSlider->setMaximum(frames -1);
    ui->framesAvailableProgressBar->setValue(frames);
}

void MainWindow::saveReferenceFrame(void) {
    QString filename = "ReferenceFrame_Frame_"
            + QString::number((ui->horizontalSlider->value()+1))
            + "_"
            + ui->referenceTypeComboBox->currentText().split(".").at(0)
            + ".png";

    referenceFrame.image->save(filename);
}

void MainWindow::saveProcessedFrame(void) {
    QString filename = ui->processedVideoComboBox->currentText().split(".").at(0)
            + "_Frame_"
            + QString::number((ui->horizontalSlider->value()+1))
            + "_"
            + ui->processedTypeComboBox->currentText()
            + ".png";

    processedFrame.image->save(filename);
}

void MainWindow::startAutoVideo(void) {
    if(ui->horizontalSlider->value() < frames) {
        autoVideo = true;
        QTimer::singleShot(1000/ui->framesSpinBox->value(), this, SLOT(setNextFrame()));
        ui->playPushButton->setEnabled(false);
    }
}

void MainWindow::stopAutoVideo(void) {
    autoVideo = false;
    ui->playPushButton->setEnabled(true);
}

void MainWindow::setNextFrame(void) {
    if(autoVideo && ui->horizontalSlider->value()+1 < frames) {
        ui->horizontalSlider->setValue(ui->horizontalSlider->value()+1);
        QTimer::singleShot(1000/ui->framesSpinBox->value(), this, SLOT(setNextFrame()));
        ui->playPushButton->setEnabled(false);
    } else {
        autoVideo = false;
        ui->playPushButton->setEnabled(true);
    }
}

void MainWindow::firstFrame(void) {
    ui->horizontalSlider->setValue(0);
    autoVideo = false;
    ui->playPushButton->setEnabled(true);
}

