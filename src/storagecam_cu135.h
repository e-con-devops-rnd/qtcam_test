/*
 * seecam_cu135.h -- Handling special feature of seecamcu135 camera
 * Copyright © 2015  e-con Systems India Pvt. Limited
 *
 * This file is part of Qtcam.
 *
 * Qtcam is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * Qtcam is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qtcam. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef STORAGECAM_CU135_H
#define STORAGECAM_CU135_H

#include <QObject>
#include "uvccamera.h"
#include <QtConcurrentRun>

#define EXPOSURECOMP_MIN 8000
#define EXPOSURECOMP_MAX 1000000
#define EXPOSURECOMP_MIN 8000
#define EXPOSURECOMP_MAX 1000000

/* STORAGECAM_CU135 */
#define CAMERA_CONTROL_STORECAM_1335       0x95

#define SET_SPECIAL_EFFECT_MODE_STORECAM_1335    0x04
#define GET_SPECIAL_EFFECT_MODE_STORECAM_1335    0x03

#define SET_SCENE_MODE_STORECAM_1335             0x02
#define GET_SCENE_MODE_STORECAM_1335             0x01

#define SET_DENOISE_CONTROL_STORECAM_1335        0x06
#define GET_DENOISE_CONTROL_STORECAM_1335        0x05

#define GET_Q_FACTOR_STORECAM_1335               0x0B
#define SET_Q_FACTOR_STORECAM_1335		 0x0C

#define SET_HDR_MODE_STORECAM_1335		 0x0A
#define GET_HDR_MODE_STORECAM_1335		 0x09

#define SET_STREAM_MODE_STORECAM_1335            0x0E
#define GET_STREAM_MODE_STORECAM_1335            0x0D

#define SET_ORIENTATION_STORECAM_1335            0x11
#define GET_ORIENTATION_STORECAM_1335            0x10

#define SET_EXPOSURE_COMPENSATION_STORECAM_1335  0x17
#define GET_EXPOSURE_COMPENSATION_STORECAM_1335  0x16

#define SET_EXP_ROI_MODE_STORECAM_1335                     0x08
#define GET_EXP_ROI_MODE_STORECAM_1335                     0x07

#define SET_FRAME_RATE_STORECAM_1335                       0x19
#define GET_FRAME_RATE_STORECAM_1335                       0x18

#define SET_FACE_DETECT_STORECAM_1335                      0x13
#define GET_FACE_DETECT_STORECAM_1335                      0x12
#define ENABLE_FACE_RECT_STORECAM_1335                     0x01
#define DISABLE_FACE_RECT_STORECAM_1335                    0x00
#define ENABLE_EMBED_DATA_STORECAM_1335                    0x01
#define DISABLE_EMBED_DATA_STORECAM_1335                   0x00


#define ENABLE_OVERLAY_RECT_STORECAM_1335                  0x01
#define DISABLE_OVERLAY_RECT_STORECAM_1335                 0x00

#define SET_SMILE_DETECTION_STORECAM_1335                  0x15
#define GET_SMILE_DETECTION_STORECAM_1335                  0x14
#define ENABLE_SMILE_DETECT_STORECAM_1335                  0x01
#define DISABLE_SMILE_DETECT_STORECAM_1335                 0x00

#define GRAB_PREVIEW_FRAME                        0x1A
#define GRAB_STILL_FRAME                          0x1E
#define STORE_FRAME                               0x1B
#define QUERY_NEXT_FRAME                          0x01
#define STORE_PREV_FRAME                          0x01
#define STORE_STILL_FRAME                         0x02

#define SET_STILL_RESOLUTION                      0x1D
#define GET_STILL_RESOLUTION                      0x1C

#define GET_NUM_OF_FRAMES_STORE                   0x1F

#define ENABLE_FACE_RECTANGLE_STORECAM_1335        0x01
#define DISABLE_FACE_RECTANGLE_STORECAM_1335       0x00
#define SET_FACE_RECT_STATE                        0x20

#define SET_TO_DEFAULT_STORECAM_1335               0x0F

#define SET_FAIL		0x00
#define SET_SUCCESS		0x01

#define GET_FAIL		0x00
#define GET_SUCCESS		0x01



class STORAGECAM_CU135 : public QObject
{
    Q_OBJECT

private:
    uvccamera uvc;

    unsigned char g_out_packet_buf[BUFFER_LENGTH];
    unsigned char g_in_packet_buf[BUFFER_LENGTH];

    void initializeBuffers();

public:
    explicit STORAGECAM_CU135(QObject *parent = 0);
    QString _title;
    QString _text;

    enum sceneModes {
        SCENE_NORMAL = 0x01,
        SCENE_DOCUMENT = 0x0C
    };
    Q_ENUMS(sceneModes)

    enum specialEffects {
        EFFECT_NORMAL = 0x01,
        EFFECT_BLACK_WHITE = 0x04,
        EFFECT_GREYSCALE = 0x07,
        EFFECT_NEGATIVE = 0x08,
        EFFECT_SKETCH = 0x10
    };

     Q_ENUMS(specialEffects)

    enum camiHDRMode {
        HdrOff = 0x01,
        HdrAuto = 0x02,
        HdrManual = 0x03
    };
    Q_ENUMS(camiHDRMode)

    enum camStreamMode {
        STREAM_MASTER_CONTINUOUS = 0x00,
        STREAM_MASTER_ONDEMAND = 0x01,
        STREAM_SOFTWARE_TRIGGER = 0x02,
        STREAM_HARDWARE_TRIGGER = 0x03
    };
    Q_ENUMS(camStreamMode)

    enum camROIAutoExpMode {
        AutoExpFace = 0x00,
        AutoExpFull = 0x01,
        AutoExpManual = 0x02,
        AutoExpDisabled = 0x03
    };
    Q_ENUMS(camROIAutoExpMode)

    enum camFaceRectMode {
        FaceRectEnable = 0x01,
        FaceRectDisable = 0x00
    };
    Q_ENUMS(camFaceRectMode)

    enum camFaceDetectEmbedDataMode {
        FaceDetectEmbedDataEnable = 0x01,
        FaceDetectEmbedDataDisable = 0x00
    };
    Q_ENUMS(camFaceDetectEmbedDataMode)

    enum camFaceDetectOverlayRect {
        FaceDetectOverlayRectEnable = 0x01,
        FaceDetectOverlayRectDisable = 0x00
    };
    Q_ENUMS(camFaceDetectOverlayRect)

    enum camSmileDetectMode {
        SmileDetectEnable = 0x01,
        SmileDetectDisable = 0x00
    };
    Q_ENUMS(camSmileDetectMode)

    enum camSmileDetectEmbedDataMode {
        SmileDetectEmbedDataEnable = 0x01,
        SmileDetectEmbedDataDisable = 0x00
    };
    Q_ENUMS(camSmileDetectEmbedDataMode)

    enum flipMirrorControls{
        SetBothFlipDisable,
        SetHorzFlip,
        SetVertiFlip,
        SetBothFlipEnable
    };
    Q_ENUMS(flipMirrorControls)


signals:
     void sceneModeValue(uint sceneMode);
     void effectModeValue(uint effectMode);
     void denoiseValue(uint denoiseVal);
     void qFactorValue(uint qFactor);
     void hdrModeValue(uint hdrMode, uint hdrValue);
     void streamModeValue(uint streamMode);
     void flipMirrorModeChanged(uint flipMirrorMode);
     void burstLengthValue(uint burstLength);
     void roiAutoExpMode(uint roiMode, uint winSize);
     void exposureCompValue(uint exposureCompensation);
     void indicateCommandStatus(QString title, QString text);
     void frameRateCtrlValue(uint frameRateVal);
     void faceDetectModeValue(uint faceDetectMode, uint faceDetectEmbedDataValue, uint faceDetectOverlayRect);
     void smileDetectModeValue(uint smileDetectMode, uint smileDetectEmbedDataValue);
     void indicateSmileThresholdRangeFailure(QString title, QString text);
     void indicateExposureValueRangeFailure(QString title, QString text);
     void numberOfframesStoreCapacity(uint numberOfFrames);
     void stillsettingsReceived(uint stillformat, uint stillResolution);
     void grabStillFrameStatus(bool grabStatus);
     void storeStillFrameSucceess();
     void titleTextChanged(QString _title,QString _text);


public slots:
    bool setSceneMode(sceneModes sceneMode);
    bool getSceneMode();

    bool getEffectMode();
    bool setEffectMode(specialEffects specialEffect);

    bool setDenoiseValue(uint deNoiseVal);
    bool getDenoiseValue();

    bool setQFactor(uint qFactor);
    bool getQFactor();

    bool setiHDRMode(camiHDRMode iHDRMode, uint iHDRValue);
    bool getiHDRMode();

    bool setStreamMode(camStreamMode streamMode);
    bool getStreamMode();

    bool setOrientation(bool horzModeSel, bool vertiModeSel);
    bool getOrientation();

    bool setROIAutoExposure(camROIAutoExpMode see3camAutoexpROIMode, uint vidResolnWidth, uint vidResolnHeight, uint xCord, uint yCord, QString winSize);
    bool getAutoExpROIModeAndWindowSize();

    bool setExposureCompensation(unsigned int exposureCompValue);
    bool getExposureCompensation();

    bool setFrameRateCtrlValue(uint frameRate);
    bool getFrameRateCtrlValue();

    bool setFaceDetectionRect(bool enableFaceDetectRect, bool embedData, bool overlayRect);
    bool getFaceDetectMode();

    bool setSmileDetection(bool enableSmileDetect, bool embedData);
    bool getSmileDetectMode();   

    bool grabPreviewFrame();
    bool storePreviewFrame();
    bool storeStillFrame(uint stillformatId,uint stillresolutionId);

    bool getNumberOfFramesCanStore(uint stillformatId,uint stillresolutionId);
    bool grabStillFrame(uint frameIndex, uint stillformatId, uint stillresolutionId);

    bool setStillResolution(uint stillformatId, uint stillresolutionId);
    bool getStillResolution();

    bool setFaceRectState(bool enableFaceRect);

    bool setToDefault();
    bool initStoreCAM1335(char *hidDeviceName);
    void deinitStoreCAM1335();
    bool readFirmwareVersion(uint *pMajorVersion, uint *pMinorVersion1, uint *sdkVersion, uint *svnVersion);

};

#endif // STORAGECAM_CU135_H
