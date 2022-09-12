#ifndef NILECAM20_USB_H
#define NILECAM20_USB_H

#include <QObject>
#include <QString>
#include "uvccamera.h"

#define CAMERA_CONTROL_NILECAM20_USB 0xB9

#define EXPOSURECOMP_MIN 8000
#define EXPOSURECOMP_MAX 1000000

#define GET_SENSOR_MODE_NILECAM20_USB 0x01
#define SET_SENSOR_MODE_NILECAM20_USB 0x02

#define GET_CAMERA_MODE_NILECAM20_USB 0x03
#define SET_CAMERA_MODE_NILECAM20_USB 0x04

#define GET_AUTO_EXPOSURE_ROI_MODE_NILECAM20_USB 0x05
#define SET_AUTO_EXPOSURE_ROI_MODE_NILECAM20_USB 0x06

#define GET_SPECIAL_MODE_NILECAM20_USB 0x07
#define SET_SPECIAL_MODE_NILECAM20_USB 0x08

#define GET_ORIENTATION_NILECAM20_USB 0x09
#define SET_ORIENTATION_NILECAM20_USB 0x0A

#define NORMAL_NILECAM20_USB 0X01
#define VERTICAL_FLIP_NILECAM20_USB 0x02
#define HORIZONTAL_MIRROR_NILECAM20_USB 0x03
#define ROTATE_NILECAM20_USB 180

#define GET_STROBE_MODE_NILECAM20_USB 0x0B
#define SET_STROBE_MODE_NILECAM20_USB 0x0C

#define GET_COLOR_KILL_MODE_NILECAM20_USB 0x0D
#define SET_COLOR_KILL_MODE_NILECAM20_USB 0x0E

#define GET_IMAGE_BURST_NILECAM20_USB 0x0F
#define SET_IMAGE_BURST_NILECAM20_USB 0x10

#define GET_FLICKER_MODE_NILECAM20_USB 0x11
#define SET_FLICKER_MODE_NILECAM20_USB 0x12

#define GET_LSC_MODES_NILECAM20_USB 0x13
#define SET_LSC_MODES_NILECAM20_USB 0x14

#define GET_DENOISE_NILECAM20_USB 0x15
#define SET_DENOISE_NILECAM20_USB 0x16

#define GET_EXPOSURE_COMPENSATION_NILECAM20_USB 0x17
#define SET_EXPOSURE_COMPENSATION_NILECAM20_USB 0x18

#define SET_DEFAULT_NILECAM20_USB 0xFF


class NILECAM20_USB : public QObject
{
   Q_OBJECT

private:
    unsigned char g_out_packet_buf[BUFFER_LENGTH];
    unsigned char g_in_packet_buf[BUFFER_LENGTH];
    uvccamera uvc;

    void initializeBuffers();

    NILECAM20_USB();

public:
    enum ROIAutoExpMode {
        CENTERED = 0x01,
        AUTO_EXP_MANNUAL = 0x02
    };
    Q_ENUMS(ROIAutoExpMode)

    enum specialModes {
        SPECIAL_NORMAL = 0x01,
        SPECIAL_GREYSCALE  = 0x02
    };
    Q_ENUMS(specialModes)

    enum Orientation{
        Normal = 0x01,
        VerticalFlip=0x02,
        HorizontalMirror=0x03,
        Rotate180 =0x04
    };
    Q_ENUMS(Orientation)

    enum strobeValues{
        FLASH_FOR_VIDEO_STREAMING = 0x01,
        FLASH_FOR_EXT_STILL_TRIGGER = 0x02,
        TORCH =0x03,
        OFF =0x04
    };
    Q_ENUMS(strobeValues)

    enum ColorKillValue{
        ColorKillValue = 0x0D
    };
    Q_ENUMS(ColorKillValue)

    enum ImageCapture{
        ImageBurst = 0x0F
    };
    Q_ENUMS(ImageCapture)

    enum FlickerMode{
        FlickerAuto =0x01,
        Manual50Hz = 0x02,
        Manual60Hz = 0x03
    };
    Q_ENUMS(FlickerMode)

    enum lscModes {
        LSCModesAuto = 0x01,
        DayLight = 0x02,
        CWFLight =0x03,
        ALight = 0x04
    };
    Q_ENUMS(lscModes)

    enum Denoise{
        Enable = 0x01,
        Disable = 0x02
    };
    Q_ENUMS(Denoise)


    enum ExposureCompensation{
        ExposureCompensation = 0x17
    };
    Q_ENUMS(ExposureCompensation)


    enum sensorModes {
        SENSOR_STANDARD = 0x01,
        SENSOR_HDR  = 0x02
    };
    Q_ENUMS(sensorModes)

    enum cameraModes {
        CAMERA_MASTER = 0x01,
        CAMERA_SLAVE  = 0x02
    };
    Q_ENUMS(cameraModes)

    signals:
        void sensorModeReceived(int effectMode);
        void cameraModeReceived(int cameraModes);
        void specialEffectMode(int effectMode);
        void flipMirrorModeChanged(int flipMirrorMode);
        void strobeModeChanged(int strobeValues);
        void sendDenoiseValue(int denoiseValue);
        void indicateCommandStatus(QString title, QString text);
        void exposureCompValueReceived(int exposureCompensation);
        void burstLengthValue(int burstLength);
        void colorKillValueChanged(int ColorKillValue);
        void roiAutoExpModeValue(int roiMode, int winSize);
        void lscModeChanged(int lscMode);
        void flickerDetectionMode(int flickerMode);
        void indicateExposureValueRangeFailure(QString title, QString text);

    public slots:
        bool setDenoiseCtrlMode(int deNoiseVal);
        bool getDenoiseValueNileCam20USB();

        bool setSpecialMode(specialModes specialEffect);
        bool getSpecialEffectsNileCam20USB();

        bool getStrobeModeNileCam20USB();
        bool setStrobeModeNileCam20USB(strobeValues strobeMode);

        bool setROIAutoExposure(ROIAutoExpMode see3camAutoexpROIMode, uint vidResolnWidth, uint vidResolnHeight, uint xCord, uint yCord, QString winSize);
        bool getAutoExpROIModeAndWindowSize();

        bool setSensorMode(sensorModes sensorMode);
        bool getSensorModeNileCam20USB();

        bool setCameraMode(cameraModes cameraMode);
        bool getCameraModeNileCam20USB();

        bool setOrientation(int orientationMode);
        bool getOrientation();

        bool getImageCaptureNileCam20USB();
        bool setBurstLength(int burstLength);

        bool setExposureCompensation(unsigned int exposureCompValue);
        bool getExposureCompensation();

        bool getColourKillModeNileCam20USB();
        bool setColourKill(uint colourKillValue);

        bool getFlickerModeNileCam20USB();
        bool setAntiFlickerMode(FlickerMode flickerMode);

        bool setLSCMode(lscModes lscMode);
        bool getLSCModeNileCam20USB();

        bool restoreDefaultNileCam20USB();

};



#endif // NILECAM20_USB_H
