/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#ifndef PARAMETERWINDOW_H
#define PARAMETERWINDOW_H

#include <QFile>
#include <QDialog>

#include "resolutionidentifier.h"

#include "yuvvideo.h"

namespace Ui {
class VideoBytes : public QThread {
    Q_OBJECT
public:
    VideoBytes(QString path):valid(false),path(path){

    }

    void run(void) {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly)){
            valid = false;
            emit done();
            return;
        }

        bytes = file.readAll();

        file.close();
        valid = true;

        emit done();
    }

    int size(void) {
        return bytes.size();
    }

    bool valid;

    QString path;
    QByteArray bytes;

signals:
    void done(void);
};

class ParameterWindow;
}

class ParameterWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ParameterWindow(QWidget *parent = 0);
    ~ParameterWindow();

    QString referencePath;
    QString processedPath;

    int firstFrame;
    int lastFrame;

signals:
    void startAnalysis(QByteArray referenceVideo, QString referenceName, QList<QByteArray> processedVideos, QList<QString> processedNames, int width,int height, int startFrame, int lastFrame);
private slots:
    void referenceVideoSelected(void);
    void processedVideoSelected(void);

    void referenceVideoPathChanged(QString videoPath);

    void referenceResolutionRadioButtonChange(void);
    void referenceResolutionChange(void);

    void referenceFirstFrameChange(int value);
    void referenceLastFrameChange(int value);

    void referenceVideoValid(void);

    void referenceResIdentified(int width, int height);

    void referenceResNotIdentifiable(void);

    void startAnalysisTriggered(void);

    void deleteSelected(void);

    void videoBytesLoaded(void);
private:    
    Ui::ParameterWindow *ui;

    QSharedPointer<ResolutionIdentifier> resIdentifier;
    QSharedPointer<Ui::VideoBytes> referenceVideo;

    QList<QSharedPointer<Ui::VideoBytes> > processedVideos;

    QString selectedPath;
};

#endif // PARAMETERWINDOW_H
