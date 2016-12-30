#-------------------------------------------------
#
# Project created by QtCreator 2016-12-22T15:28:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = facedetector_1222
TEMPLATE = app

INCLUDEPATH += /usr/local/include/opencv
LIBS += -L/usr/local/lib -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_viz -lopencv_core

SOURCES += main.cpp\
        mainwindow.cpp \
    imageloader.cpp

HEADERS  += mainwindow.h \
    imageloader.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
