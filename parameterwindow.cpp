/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#include "parameterwindow.h"
#include "ui_parameterwindow.h"

#include <QFileDialog>

#include <QDebug>
#include <QMessageBox>

ParameterWindow::ParameterWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ParameterWindow)
{
    ui->setupUi(this);

    connect(ui->chooseReferencePathToolButton,SIGNAL(clicked(bool)), this,SLOT(referenceVideoSelected(void)));
    connect(ui->chooseProcessedPathToolButton,SIGNAL(clicked(bool)),this,SLOT(processedVideoSelected(void)));

    connect(ui->referenceVideoLineEdit, SIGNAL(textChanged(QString)),this,SLOT(referenceVideoPathChanged(QString)));

    connect(ui->referenceResKnownRadioButton,SIGNAL(clicked(bool)),this,SLOT(referenceResolutionRadioButtonChange()));

    connect(ui->referenceResNotKnownRadioButton,SIGNAL(clicked(bool)),this,SLOT(referenceResolutionRadioButtonChange()));

    connect(ui->referenceResWidthSpinBox,SIGNAL(valueChanged(int)),this,SLOT(referenceResolutionChange()));
    connect(ui->referenceResHeightSpinBox,SIGNAL(valueChanged(int)),this,SLOT(referenceResolutionChange()));

    connect(ui->referenceFirstFrameSpinBox,SIGNAL(valueChanged(int)),this,SLOT(referenceFirstFrameChange(int)));
    connect(ui->referenceLastFrameSpinBox,SIGNAL(valueChanged(int)),this,SLOT(referenceLastFrameChange(int)));

    connect(ui->startAnalysisPushButton,SIGNAL(clicked(bool)),this,SLOT(startAnalysisTriggered()));

    connect(ui->deleteSelectedPushButton,SIGNAL(clicked(bool)),this,SLOT(deleteSelected()));
    ui->referenceResWidthSpinBox->setEnabled(true);
    ui->referenceResHeightSpinBox->setEnabled(true);

    ui->referenceFirstFrameSpinBox->setEnabled(true);
    ui->referenceLastFrameSpinBox->setEnabled(true);

    ui->startAnalysisPushButton->setEnabled(false);
    ui->pleaseWaitLabel->setVisible(false);
    resIdentifier = QSharedPointer<ResolutionIdentifier>::create();

    selectedPath = QDir::homePath();
}

ParameterWindow::~ParameterWindow() {
    delete ui;
}

void ParameterWindow::referenceVideoSelected(void) {
    auto videoPath = QFileDialog::getOpenFileName(this,tr("Select the reference YUV video"), selectedPath);
    if(videoPath != "") {
        ui->referenceVideoLineEdit->setText(videoPath);
        selectedPath = videoPath;
    }
}

void ParameterWindow::processedVideoSelected(void) {
    QFileDialog dialog(this);
    dialog.setDirectory(selectedPath);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        qDebug() << fileNames;
    }
    for(auto s : fileNames) {
        ui->startAnalysisPushButton->setEnabled(false);
        processedVideos.append(QSharedPointer<Ui::VideoBytes>::create(s));
        processedVideos.last()->start();
        ui->pleaseWaitLabel->setVisible(true);
        connect(processedVideos.last().data(),SIGNAL(done()),this,SLOT(videoBytesLoaded()));
        ui->processedVideosListWidget->addItem(s);
    }
}

void ParameterWindow::referenceVideoPathChanged(QString videoPath) {
    referencePath = videoPath;
    referenceVideo = QSharedPointer<Ui::VideoBytes>::create(videoPath);

    connect(referenceVideo.data(),SIGNAL(done()),this,SLOT(referenceVideoValid()));
    referenceVideo->start();
}

void ParameterWindow::referenceVideoValid(void) {
    if(!referenceVideo.isNull() && referenceVideo->valid ) {
        ui->referenceAttributesGroupBox->setEnabled(true);
        ui->referenceVideoBytesLabel->setText(QString::number(referenceVideo->size()));
    }else{
        ui->referenceAttributesGroupBox->setEnabled(false);
        ui->referenceVideoBytesLabel->setText("0");
    }
}

void ParameterWindow::referenceResolutionRadioButtonChange(void) {
    ui->referenceResWidthSpinBox->setEnabled(ui->referenceResKnownRadioButton->isChecked());
    ui->referenceResHeightSpinBox->setEnabled(ui->referenceResKnownRadioButton->isChecked());

    if(ui->referenceResNotKnownRadioButton->isChecked()) {
        connect(resIdentifier.data(),SIGNAL(resolutionIdentified(int,int)),this,SLOT(referenceResIdentified(int,int)));
        connect(resIdentifier.data(),SIGNAL(resolutionNotIdentifiable()),this,SLOT(referenceResNotIdentifiable()));
        resIdentifier->show();
        resIdentifier->identifyResolution(referenceVideo->bytes);
    }
}

void ParameterWindow::referenceResolutionChange(void) {
    int width = ui->referenceResWidthSpinBox->value();
    int height = ui->referenceResHeightSpinBox->value();

    if(referenceVideo->size() % (width*height*3)/2 == 0) {
        int frames =referenceVideo->size() / ((width*height*3)/2);
        ui->referenceVideoFramesLabel->setText(QString::number(frames));
        ui->referenceLastFrameSpinBox->setMaximum(frames);
        if(ui->referenceLastFrameSpinBox->value() > frames) {
            ui->referenceLastFrameSpinBox->setValue(frames);
        }
    }

    videoBytesLoaded();
}

void ParameterWindow::referenceFirstFrameChange(int value) {
    if(value > ui->referenceLastFrameSpinBox->value()) {
        ui->referenceFirstFrameSpinBox->setValue(ui->referenceLastFrameSpinBox->value());
    }
}

void ParameterWindow::referenceLastFrameChange(int value) {
    if(value < ui->referenceFirstFrameSpinBox->value()) {
        ui->referenceLastFrameSpinBox->setValue(ui->referenceFirstFrameSpinBox->value());
    }
}

void ParameterWindow::referenceResIdentified(int width, int height) {
    resIdentifier->disconnect();
    ui->referenceResWidthSpinBox->setValue(width);
    ui->referenceResHeightSpinBox->setValue(height);
    resIdentifier->hide();
    ui->referenceResKnownRadioButton->setChecked(true);
    ui->referenceResWidthSpinBox->setEnabled(true);
    ui->referenceResHeightSpinBox->setEnabled(true);
}

void ParameterWindow::referenceResNotIdentifiable(void) {
    resIdentifier->disconnect();
    resIdentifier->hide();

    QMessageBox msgBox;
    msgBox.setText("I NEED RESOLUTIONS!!!");
    msgBox.setInformativeText("Without any information about the resolution, the video can not be analysed! \n\n Application will be closed!");
    msgBox.exec();
    close();
}

void ParameterWindow::startAnalysisTriggered(void) {  
    QList<QByteArray> processedVideos;
    QList<QString>    processedNames;

    int i=0;
    for(auto videobyte : this->processedVideos) {
        processedVideos.append(videobyte->bytes);

        auto item = ui->processedVideosListWidget->item(i);
        processedNames.append(item->text().split("/").last());

        i++;
    }

    QString referenceName    = ui->referenceVideoLineEdit->text().split("/").last();

    unsigned int width      = ui->referenceResWidthSpinBox->value();
    unsigned int height     = ui->referenceResHeightSpinBox->value();
    unsigned int firstFrame = ui->referenceFirstFrameSpinBox->value();
    unsigned int lastFrame  = ui->referenceLastFrameSpinBox->value();

    emit startAnalysis(referenceVideo->bytes, referenceName, processedVideos, processedNames, width, height, firstFrame, lastFrame);
    hide();
}

void ParameterWindow::deleteSelected(void) {
    auto items = ui->processedVideosListWidget->selectedItems();

    foreach(auto item, items) {
        foreach(auto video, processedVideos) {
            if(video->path == item->text()) {
                processedVideos.removeOne(video);
                break;
            }
        }

        delete ui->processedVideosListWidget->takeItem(ui->processedVideosListWidget->row(item));
    }
}

void ParameterWindow::videoBytesLoaded(void) {
    bool valid = true;
    for(auto bytes : processedVideos) {
        valid = valid & bytes->valid;
    }

    if(processedVideos.size() > 0 && !referenceVideo.isNull() && referenceVideo->valid && ui->referenceResWidthSpinBox->value() > 1 &&
        ui->referenceResHeightSpinBox->value() > 1 && valid) {
        ui->startAnalysisPushButton->setEnabled(true);
        ui->pleaseWaitLabel->setVisible(false);
    } else {
       ui->startAnalysisPushButton->setEnabled(false);
    }
}
