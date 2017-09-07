/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMovie>

#include "yuvvideo.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int frames;

    bool autoVideo;
private:
    Ui::MainWindow *ui;

    QSharedPointer<QGraphicsScene> referenceScene;
    QSharedPointer<QGraphicsScene> processedScene;

    QSharedPointer<YuvVideo> referenceVideo;
    QList<QSharedPointer<YuvVideo> > processedVideo;

    YuvFrame::Frame referenceFrame;
    YuvFrame::Frame processedFrame;

    bool connectedZoom;
public slots:
    void frameLoaded(void);

    void displayVideo(QSharedPointer<YuvVideo> referenceVideo, QList<QSharedPointer<YuvVideo> > processedVideo, unsigned int startFrame, unsigned int lastFrame);

    void saveReferenceFrame(void);
    void saveProcessedFrame(void);

    void displayNewFrame(void);

    void startAutoVideo(void);
    void stopAutoVideo(void);
    void firstFrame(void);

    void setNextFrame(void);
};

#endif // MAINWINDOW_H
