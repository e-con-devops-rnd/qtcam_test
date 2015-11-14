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


void ASCELLA::setLEDStatusMode(camLedMode ledMode, QString brightnessVal){

    int bytesSent;
    u_int8_t brightnessIntVal;

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

        brightnessIntVal = brightnessVal.toInt();

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

void ASCELLA::setAutoFocusMode(camAfMode afMode){

    int bytesSent;

    if(uvccamera::handle == NULL)
    {
        return void();
    }

    if(afMode == Continuous || afMode == OneShot)
    {
        memset(g_out_packet_buf, 0x00, ASCELLA_BUFLEN);

        g_out_packet_buf[1] = 0x02;
        g_out_packet_buf[2] = 0x04;
        if(afMode == Continuous)
            g_out_packet_buf[3] = 0x03;
        else if(afMode == OneShot)
            g_out_packet_buf[3] = 0x00;

        qDebug()<<"afmode "<<afMode;
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

void ASCELLA::setExposureCompensation(QString exposureVal){

    int bytesSent;
    u_int8_t exposureIntVal;

    if(uvccamera::handle == NULL)
    {
        return void();
    }

    memset(g_out_packet_buf, 0x00, ASCELLA_BUFLEN);

    g_out_packet_buf[1] = 0x02;
    g_out_packet_buf[2] = 0x03;

    exposureIntVal = exposureVal.toInt();

    g_out_packet_buf[3] = (unsigned char)(exposureIntVal & 0xFF);

    qDebug()<<"exposureIntVal "<<exposureIntVal;
    bytesSent = libusb_control_transfer(uvccamera::handle,
                                        0x21,
                                        0x09,
                                        0x200,
                                        0x2,
                                        g_out_packet_buf,
                                        ASCELLA_BUFLEN,
                                        ASCELLA_TIMEOUT);
    qDebug()<<"bytesSent"<<bytesSent;
    if(0 > bytesSent){
        return void();
    }
}


void ASCELLA::setSceneMode(camSceneMode sceneMode){

    int bytesSent;

    if(uvccamera::handle == NULL)
    {
        return void();
    }

    if(sceneMode == SceneNormal || sceneMode == SceneDocScan){
        memset(g_out_packet_buf, 0x00, ASCELLA_BUFLEN);

        g_out_packet_buf[1] = 0x02;
        g_out_packet_buf[2] = 0x09;
        if(sceneMode == 0x01)
            g_out_packet_buf[3] = 0x00;
        else if(sceneMode == 0x02)
            g_out_packet_buf[3] = 0x20;

        bytesSent = libusb_control_transfer(uvccamera::handle,
                                            0x21,
                                            0x09,
                                            0x200,
                                            0x2,
                                            g_out_packet_buf,
                                            ASCELLA_BUFLEN,
                                            ASCELLA_TIMEOUT);
        qDebug()<<"bytesSent"<<bytesSent;
        if(0 > bytesSent){
            return void();
        }
    }
    else{
        return void();
    }

}


void ASCELLA::setNoiseReduceMode(camNoiseReduceMode NoiseReduceMode, QString NoiseReduceFixVal){

    int bytesSent;
    u_int8_t noiseReduceFixIntVal;

    if(uvccamera::handle == NULL){
        return void();
    }
    if(NoiseReduceMode == NoiseReduceNormal || NoiseReduceMode == NoiseReduceFix){
        memset(g_out_packet_buf, 0x00, ASCELLA_BUFLEN);

        g_out_packet_buf[1] = 2;
        g_out_packet_buf[2] = 8;
        if(NoiseReduceMode == NoiseReduceNormal)
            g_out_packet_buf[3] = 0x00;
        else if(NoiseReduceMode == NoiseReduceFix){
            noiseReduceFixIntVal = NoiseReduceFixVal.toInt();
            noiseReduceFixIntVal |= 0x80;
            g_out_packet_buf[3] = (unsigned char)(noiseReduceFixIntVal & 0xFF);
        }

        bytesSent = libusb_control_transfer(uvccamera::handle,
                                            0x21,
                                            0x09,
                                            0x200,
                                            0x2,
                                            g_out_packet_buf,
                                            ASCELLA_BUFLEN,
                                            ASCELLA_TIMEOUT);
        qDebug()<<"setnoiseReduceMode:bytesSent"<<bytesSent;
        if(0 > bytesSent){
            return void();
        }
    }
    else{
        return void();
    }
}

void ASCELLA::setLimitMaxFrameRateMode(camLimitMaxFRMode LimitMaxFRMode, QString maxFrameRateVal){

    int bytesSent;
    u_int8_t maxFRIntVal;

    if(uvccamera::handle == NULL){
        return void();
    }
    if(LimitMaxFRMode == Disable || LimitMaxFRMode == ApplyMaxFrameRate){
        memset(g_out_packet_buf, 0x00, ASCELLA_BUFLEN);

        g_out_packet_buf[1] = 2;
        g_out_packet_buf[2] = 10;
        if(LimitMaxFRMode == Disable)
            g_out_packet_buf[3] = 0x00;
        else if(LimitMaxFRMode == ApplyMaxFrameRate){
            maxFRIntVal = maxFrameRateVal.toInt();
            g_out_packet_buf[3] = (unsigned char)(maxFRIntVal & 0xFF);
        }
        qDebug()<< "g_out_packet_buf[3]" << g_out_packet_buf[3];
        bytesSent = libusb_control_transfer(uvccamera::handle,
                                            0x21,
                                            0x09,
                                            0x200,
                                            0x2,
                                            g_out_packet_buf,
                                            ASCELLA_BUFLEN,
                                            ASCELLA_TIMEOUT);
        qDebug()<<"setLimitMaxFrameRateMode:bytesSent"<<bytesSent;
        if(0 > bytesSent){
            return void();
        }
    }
    else{
        return void();
    }
}

void ASCELLA::setColorMode(camColorMode colorMode, QString blackwhiteThreshold){

    int bytesSent;
    u_int8_t bwThresholdIntVal;

    if(uvccamera::handle == NULL){
        return void();
    }
    if(colorMode == ColorModeNormal || colorMode == ColorModeMono || colorMode == ColorModeNegative || colorMode == ColorModeBlackWhite){
        memset(g_out_packet_buf, 0x00, ASCELLA_BUFLEN);

        g_out_packet_buf[1] = 0x06;
        g_out_packet_buf[2] = 0x06;
        if(colorMode == ColorModeNormal)
            g_out_packet_buf[3] = 0x00;
        else if(colorMode == ColorModeMono)
            g_out_packet_buf[3] = 0x01;
        else if(colorMode == ColorModeNegative)
            g_out_packet_buf[3] = 0x03;
        else if(colorMode == ColorModeBlackWhite){
            bwThresholdIntVal = blackwhiteThreshold.toInt();
            g_out_packet_buf[3] = 0x0A;
            g_out_packet_buf[4] = bwThresholdIntVal;
        }
        qDebug()<< "setColorMode:g_out_packet_buf[4]" << g_out_packet_buf[4];
        bytesSent = libusb_control_transfer(uvccamera::handle,
                                            0x21,
                                            0x09,
                                            0x200,
                                            0x2,
                                            g_out_packet_buf,
                                            ASCELLA_BUFLEN,
                                            ASCELLA_TIMEOUT);
        qDebug()<<"setColorMode:bytesSent"<<bytesSent;
        if(0 > bytesSent){
            return void();
        }
    }
    else{
        return void();
    }

}
