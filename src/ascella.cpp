/*
 * ascella.cpp -- Handling special feature of ascella camera
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
#include "ascella.h"
#include <stdio.h>


void ASCELLA::setLEDStatusMode(camledmode ledMode, QString brightnessVal){

    int bytesSent;

    if(uvccamera::handle == NULL){
        return void();
    }
    if(ledMode == LedAuto || ledMode == LedManual || ledMode == LedOff){
        memset(g_out_packet_buf, 0x00, ASCELLA_BUFLEN);

        g_out_packet_buf[0] = 0x00;
        g_out_packet_buf[1] = 0x01;
        if(ledMode == LedOff)
            g_out_packet_buf[2] = 0x00;
        else if(ledMode == LedAuto)
            g_out_packet_buf[2] = 0x01;
        else if(ledMode == LedManual)
            g_out_packet_buf[2] = 0x02;

        u_int8_t brightnessIntVal = brightnessVal.toInt();

        g_out_packet_buf[3] = (unsigned char)(brightnessIntVal & 0xFF);

        bytesSent = libusb_control_transfer(uvccamera::handle,
                                            0x21,
                                            0x09,
                                            0x200,
                                            0x2,
                                            g_out_packet_buf,
                                            ASCELLA_BUFLEN,
                                            ASCELLA_TIMEOUT);
        if(0 > bytesSent){
            return void();
        }
    }
    else{
        return void();
    }

}


void ASCELLA::setAutoFocusMode(u_int8_t afMode){

    int bytesSent;

    if(uvccamera::handle == NULL)
    {
        return void();
    }

    if(afMode == continuous || afMode == oneShot)
    {
        memset(g_out_packet_buf, 0x00, BUFFER_LENGTH);

        g_out_packet_buf[1] = 0x02;
        g_out_packet_buf[2] = 0x04;
        if(afMode == 0x01)
            g_out_packet_buf[3] = 0x03;
        else if(afMode == 0x02)
            g_out_packet_buf[3] = 0x00;

        bytesSent = libusb_control_transfer(uvccamera::handle,
                                            0x21,
                                            0x09,
                                            0x200,
                                            0x2,
                                            g_out_packet_buf,
                                            BUFFER_LENGTH,
                                            0);

        // need to check bytesSent
    }
    else{
        return void();
    }
}


