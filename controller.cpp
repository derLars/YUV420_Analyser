/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#include "controller.h"

Controller::Controller(QObject *parent) : QObject(parent) {
    mainWindow = QSharedPointer<MainWindow>::create();
    parameterWindow = QSharedPointer<ParameterWindow>::create();
    analyser = QSharedPointer<YuvAnalyser>::create();

    connect(parameterWindow.data(),SIGNAL(startAnalysis(QByteArray,QString,QList<QByteArray>,QList<QString>,int,int,int,int)),analyser.data(),SLOT(startAnalysis(QByteArray,QString,QList<QByteArray>,QList<QString>,int,int,int,int)));
    connect(analyser.data(),SIGNAL(displayVideo(QSharedPointer<YuvVideo>,QList<QSharedPointer<YuvVideo> >,uint,uint)),mainWindow.data(),SLOT(displayVideo(QSharedPointer<YuvVideo>,QList<QSharedPointer<YuvVideo> >,uint,uint)));

    parameterWindow->show();
}

