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
    Q_ENUMS(camledmode)

private:
    unsigned char g_out_packet_buf[BUFFER_LENGTH];
    uvccamera uvc;

public:
    enum camledmode {
        LedOff = 1,
        LedAuto = 2,
        LedManual = 3
    };


    enum camAfmode {
        continuous = 1,
        oneShot = 2
    };

signals:

public slots:
    /**
     * @brief   Sets the led status mode.
     * @param ledMode  set the led mode of the camera (off/Auto/Manual)
     */
    void setLEDStatusMode(camledmode ledMode, QString brightnessVal);

    /**
     * @brief   Sets the auto focus mode.
     * @param afMode  set the auto focus mode of the camera (continous/one shot)
     */
    void setAutoFocusMode(u_int8_t afMode);

};

#endif // ASCELLA_H
