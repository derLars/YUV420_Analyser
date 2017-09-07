/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "parameterwindow.h"
#include "yuvanalyser.h"
#include "mainwindow.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);

    QSharedPointer<MainWindow> mainWindow;
    QSharedPointer<ParameterWindow> parameterWindow;
    QSharedPointer<YuvAnalyser> analyser;
};

#endif // CONTROLLER_H
