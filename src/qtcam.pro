# Add more folders to ship with the application, here
folder_01.source = qml/qtcam
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

QT += widgets
TARGET = Qtcam

CONFIG += release

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    logger.cpp \
    about.cpp \
    cameraproperty.cpp \
    videostreaming.cpp \
    uvccamera.cpp \
    v4l2-api.cpp \
    seecam_10cug_m.cpp \
    seecam_10cug_bayer.cpp \
    seecam_11cug.cpp \
    seecam_cu80.cpp \
    seecam_cu50.cpp \
    seecam_ar0130.cpp \
    videoencoder.cpp \
	ms_time.c \
    seecam_cu51.cpp \
    see3cam_cu130.cpp

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()


HEADERS += \
    logger.h \
    about.h \
    cameraproperty.h \
    videostreaming.h \
    uvccamera.h \
    v4l2-api.h \
    seecam_10cug_m.h \
    seecam_10cug_bayer.h \
    seecam_11cug.h \
    seecam_cu80.h \
    seecam_cu50.h \
    seecam_ar0130.h \
    videoencoder.h \
    defs.h \
    ms_time.h \
    sound.h \
    lavc_common.h \
    seecam_ar0130.h \
    seecam_cu51.h \
    see3cam_cu130.h
    

#INCLUDEPATH +=  $$PWD/v4l2headers/include \
#                $$PWD/include

#LIBS += -lv4l2 -lv4lconvert \
#        -ludev \
#        -L$$PWD/lib -lavutil -lavcodec -lavformat -lswscale -lavresample -lportaudio -lsndfile
#INCLUDEPATH +=  avpackage/include \
#               udevpackage/include

#LIBS += -lv4l2 -lv4lconvert -ludev \
#        -L$$PWD/avpackage/lib/ -lavutil \
#        -L$$PWD/avpackage/lib/ -lavcodec \
#        -L$$PWD/avpackage/lib/ -lavformat \
#        -L$$PWD/avpackage/lib/ -lswscale \
#        -L$$PWD/avpackage/lib/ -lavresample \
#        -L$$PWD/udevpackage/lib -ludev

INCLUDEPATH +=  /home/vishnu/Music/avpackage/include \
               /home/vishnu/Music/udevpackage/include

LIBS += -lv4l2 -lv4lconvert -ludev \
        -L/home/vishnu/Music/avpackage/lib/ -lavutil \
        -L/home/vishnu/Music/avpackage/lib/ -lavcodec \
        -L/home/vishnu/Music/avpackage/lib/ -lavformat \
        -L/home/vishnu/Music/avpackage/lib/ -lswscale \
        -L/home/vishnu/Music/avpackage/lib/ -lavresample \
        -L/home/vishnu/Music/udevpackage/lib -ludev

QMAKE_CFLAGS_THREAD = -D__STDC_CONSTANT_MACROS      #For Ubuntu 12.04 compilation
QMAKE_CXXFLAGS_THREAD = -D__STDC_CONSTANT_MACROS    #For Ubuntu 12.04 compilation

OTHER_FILES += \
    qml/qtcam/UVCSettings/see3cam51/uvc51.qml
