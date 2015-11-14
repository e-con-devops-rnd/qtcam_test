/*
 * ascella.h -- Handling special feature of ascella camera
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
#ifndef ASCELLA_H
#define ASCELLA_H

#include "uvccamera.h"

class ASCELLA: public QObject {
    Q_OBJECT
    Q_ENUMS(camLedMode)
    Q_ENUMS(camAfMode)
    Q_ENUMS(camSceneMode)
    Q_ENUMS(camNoiseReduceMode)
    Q_ENUMS(camLimitMaxFRMode)
    Q_ENUMS(camColorMode)

private:
    unsigned char g_out_packet_buf[BUFFER_LENGTH];
    uvccamera uvc;

public:
    enum camLedMode {
        LedOff = 1,
        LedAuto = 2,
        LedManual = 3
    };

    enum camAfMode {
        Continuous = 1,
        OneShot = 2
    };

    enum camSceneMode {
        SceneNormal = 1,
        SceneDocScan = 2
    };

    enum camNoiseReduceMode {
        NoiseReduceNormal = 1,
        NoiseReduceFix = 2
    };

    enum camLimitMaxFRMode{
        Disable = 1,
        ApplyMaxFrameRate = 2
    };

    enum camColorMode{
        ColorModeNormal = 1,
        ColorModeMono = 2,
        ColorModeNegative = 3,
        ColorModeBlackWhite = 4
    };

signals:

public slots:
    /**
     * @brief   Sets the led status mode.
     * @param ledMode  set the led mode of the camera (off/Auto/Manual)
     * @param brightnessVal - set the brightness of led in the camera if auto or manual selected
     */
    void setLEDStatusMode(camLedMode ledMode, QString brightnessVal);

    /**
     * @brief   Sets the auto focus mode.
     * @param afMode  set the auto focus mode of the camera (continous/one shot)
     */
    void setAutoFocusMode(camAfMode afMode);

    /**
     * @brief   Sets the offset required for auto exposure.
     * @param exposureVal  set the exposure value in auto exposure in the camera.
     */
    void setExposureCompensation(QString exposureVal);

    /**
     * @brief   Sets the scene mode in the camera.
     * @param sceneMode  set the scene mode in the camera(normal/Document scanner).
     */
    void setSceneMode(camSceneMode sceneMode);

    /**
      * @brief set the noise reduction mode
      * @param noiseReduceMode - set the noise reduction mode(Auto/Fix)
      * @param noiseReduceFixVal - set the amount of noise is to be reduced when Fix mode is selected
      */
    void setNoiseReduceMode(camNoiseReduceMode noiseReduceMode, QString noiseReduceFixVal);

    /**
      * @brief set the limit maximum frame rate reduction mode
      * @param LimitMaxFRMode - set the maximium frame rate limit mode(Disable/Apply Maximum Frame Rate)
      * @param maxFrameRateVal - set the max frame rate choosen when Apply Maximum Frame Rate mode is selected
      */
    void setLimitMaxFrameRateMode(camLimitMaxFRMode LimitMaxFRMode, QString maxFrameRateVal);

    /**
      * @brief set the color mode
      * @param colorMode - set the color mode(Normal/Mono/Negative/Black and White)
      * @param blackwhiteThreshold - set the black and white threshold value
      */
    void setColorMode(camColorMode colorMode, QString blackwhiteThreshold);
};

#endif // ASCELLA_H
