#ifndef NILECAM20_USB_H
#define NILECAM20_USB_H

#include <QObject>
#include <QString>
#include "uvccamera.h"

#define CAMERA_CONTROL_NILECAM20_USB 0xB9

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

public:
    enum camROIAutoExpMode {
        Centered = 0x01,
        Manual = 0x02
    };
    Q_ENUMS(camROIAutoExpMode)

    enum GetSpecialEffect{
        NormalMode=0x01,
        GrayscaleMode=0x02
    };
    Q_ENUMS(GetSpecialEffect)

    enum SetSpecialEffect{
        NormalMode=0x01,
        GrayscaleMode=0x02
    };
    Q_ENUMS(SetSpecialEffect)

    enum GetOrientation{
        Normal = 0x01,
        VerticalFlip=0x02,
        HorizontalMirror=0x03,
        Rotate180 =0x04
    };
    Q_ENUMS(GetOrientation)

    enum SetOrientation{
        Normal =0x01,
        VerticalFlip=0x02,
        HorizontalMirror=0x03,
        Rotate180 =0x04
    };
    Q_ENUMS(SetOrientation)


    enum GetStrobe{
        FlashForVideoStreaming = 0x01,
        FlashForExtStillTrigger = 0x02,
        Torch =0x03,
        Off =0x04
    };
    Q_ENUMS(GetStrobe)

    enum SetStrobe{
        FlashForVideoStreaming = 0x01,
        FlashForExtStillTrigger = 0x02,
        Torch =0x03,
        Off =0x04
    };
    Q_ENUMS(SetStrobe)

    enum GetColorKillValue{
        getColorKillValue = 0x0D
    };
    Q_ENUMS(GetColorKillValue)

    enum SetColorKillValue{
        getColorKillValue = 0x0E
    };
    Q_ENUMS(SetColorKillValue)

    enum GetImageCapture{
        getImageBurst = 0x0F
    };
    Q_ENUMS(GetImageCapture)

    enum SetImageCapture{
        setImageBurst = 0x10
    };
    Q_ENUMS(SetImageCapture)

    enum GetFlickerMode{
        Auto =0x01,
        Manual50Hz = 0x02,
        Manual60Hz = 0x03
    };
    Q_ENUMS(GetFlickerMode)

    enum SetFlickerMode{
        Auto =0x01,
        Manual50Hz = 0x02,
        Manual60Hz = 0x03
    };
    Q_ENUMS(SetFlickerMode)


    enum GetLSCModes{
        Auto = 0x01,
        DayLight = 0x02,
        CWFLight =0x03,
        ALight = 0x04
    };
    Q_ENUMS(GetLSCModes)

    enum SetLSCModes{
        Auto = 0x01,
        DayLight = 0x02,
        CWFLight =0x03,
        ALight = 0x04
    };
    Q_ENUMS(SetLSCModes)

    enum GetDenoise{
        Enable = 0x01,
        Disable = 0x02
    };
    Q_ENUMS(GetDenoise)

    enum SetDenoise{
        Enable = 0x01,
        Disable = 0x02
    };
    Q_ENUMS(SetDenoise)


    enum camExposureCompensation{
        GetExposureCompensation = 0x17
    };
    Q_ENUMS(camExposureCompensation)

    enum SetExposureCompensation{
       setExposureCompensation = 0x18
    };
    Q_ENUMS(setExposureCompensation)




    public slots:
        bool setDenoiseValueNileCam20USB(int deNoiseVal);
        bool getDenoiseValueNileCam20USB();

        bool setSpecialEffectsNileCam20USB(const specialEffects &specialEffect);
        bool getSpecialEffectsNileCam20USB();

        bool setExpRoiModeNileCam20USB(camROIAutoExpMode see3camAutoexpROIMode, uint vidResolnWidth, uint vidResolnHeight, uint xCord, uint yCord, QString winSize);
        bool getExpRoiModeNileCam20USB();

        bool setExposureCompensationNileCam20USB(unsigned int exposureCompValue);
        bool getExposureCompensationNileCam20USB();

        bool restoreDefaultNileCam20USB();

};



#endif // NILECAM20_USB_H
