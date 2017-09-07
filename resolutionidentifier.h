/*
*   Author: Lars Schwensen
*   Date: April 2017
*/

#ifndef RESOLUTIONIDENTIFIER_H
#define RESOLUTIONIDENTIFIER_H

#include <QDialog>
#include <QList>

#include "yuvvideo.h"

namespace Ui {
class ResolutionIdentifier;
}

class ResolutionIdentifier : public QDialog
{
    Q_OBJECT

public:
    explicit ResolutionIdentifier(QWidget *parent = 0);
    ~ResolutionIdentifier();

    void identifyResolution(QByteArray videoBytes);

signals:
    void resolutionIdentified(int width, int height);
    void resolutionNotIdentifiable(void);

public slots:
    void frameLoaded(int frameNumber);
private:
    Ui::ResolutionIdentifier *ui;

    QList<int> width;
    QList<int> height;

    int index;

    int availableFrames;

    QByteArray bytes;

    QSharedPointer<YuvVideo> video;

    bool isPossible(int width, int height, int numOfBytes);
};

#endif // RESOLUTIONIDENTIFIER_H
