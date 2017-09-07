#-------------------------------------------------
#
# Project created by QtCreator 2017-05-01T10:31:14
#
#-------------------------------------------------

QT       += core gui
QT      += multimedia

CONFIG += c++14
CONFIG += static

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = YUV_Analyzer
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    yuvframe.cpp \
    yuvvideo.cpp \
    parameterwindow.cpp \
    resolutionidentifier.cpp \
    yuvanalyser.cpp \
    controller.cpp

HEADERS  += mainwindow.h \
    yuvframe.h \
    yuvvideo.h \
    yuvpixel.h \
    parameterwindow.h \
    resolutionidentifier.h \
    yuvanalyser.h \
    controller.h

FORMS    += mainwindow.ui \
    parameterwindow.ui \
    resolutionidentifier.ui
