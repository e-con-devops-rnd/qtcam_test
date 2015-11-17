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

        g_out_packet_buf[1] = 0x01;
        if(ledMode == LedOff)
            g_out_packet_buf[2] = 0x00;
        else if(ledMode == LedAuto)
            g_out_packet_buf[2] = 0x01;
        else if(ledMode == LedManual)
            g_out_packet_buf[2] = 0x02;

        qDebug()<<"ledMode"<<ledMode;

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

        g_out_packet_buf[1] = 0x04;
        if(afMode == Continuous)
            g_out_packet_buf[2] = 0x03;
        else if(afMode == OneShot)
            g_out_packet_buf[2] = 0x00;

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

    g_out_packet_buf[1] = 0x03;
    exposureIntVal = exposureVal.toInt();
    g_out_packet_buf[2] = (unsigned char)(exposureIntVal & 0xFF);

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

        g_out_packet_buf[1] = 0x09;
        if(sceneMode == 0x01)
            g_out_packet_buf[2] = 0x00;
        else if(sceneMode == 0x02)
            g_out_packet_buf[2] = 0x20;

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

        g_out_packet_buf[1] = 8;
        if(NoiseReduceMode == NoiseReduceNormal)
            g_out_packet_buf[2] = 0x00;
        else if(NoiseReduceMode == NoiseReduceFix){
            noiseReduceFixIntVal = NoiseReduceFixVal.toInt();
            noiseReduceFixIntVal |= 0x80;
            g_out_packet_buf[2] = (unsigned char)(noiseReduceFixIntVal & 0xFF);
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

        g_out_packet_buf[1] = 10;
        if(LimitMaxFRMode == Disable)
            g_out_packet_buf[2] = 0x00;
        else if(LimitMaxFRMode == ApplyMaxFrameRate){
            maxFRIntVal = maxFrameRateVal.toInt();
            g_out_packet_buf[2] = (unsigned char)(maxFRIntVal & 0xFF);
        }
        qDebug()<< "g_out_packet_buf[2]" << g_out_packet_buf[2];
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
        if(colorMode == ColorModeNormal)
            g_out_packet_buf[2] = 0x00;
        else if(colorMode == ColorModeMono)
            g_out_packet_buf[2] = 0x01;
        else if(colorMode == ColorModeNegative)
            g_out_packet_buf[2] = 0x03;
        else if(colorMode == ColorModeBlackWhite){
            bwThresholdIntVal = blackwhiteThreshold.toInt();
            g_out_packet_buf[2] = 0x0A;
            g_out_packet_buf[3] = bwThresholdIntVal;
        }
        qDebug()<< "setColorMode:g_out_packet_buf[3]" << g_out_packet_buf[3];
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


void ASCELLA::setCenterWeightedAutoFocus(){

    int bytesSent;

    if(uvccamera::handle == NULL){
        return void();
    }
    memset(g_out_packet_buf, 0x00, ASCELLA_BUFLEN);

    g_out_packet_buf[1] = 0xFF;
    g_out_packet_buf[2] = 0x01;

    qDebug()<< "setcenterweighted af:g_out_packet_buf[2]" << g_out_packet_buf[2];
    bytesSent = libusb_control_transfer(uvccamera::handle,
                                        0x21,
                                        0x09,
                                        0x200,
                                        0x2,
                                        g_out_packet_buf,
                                        ASCELLA_BUFLEN,
                                        ASCELLA_TIMEOUT);
    qDebug()<<"setcenterweightedaf:bytesSent"<<bytesSent;
    if(0 > bytesSent){
        return void();
    }
}

void ASCELLA::setCustomAreaAutoFocus(QString afHoriStart, QString afHoriEnd, QString afVertiStart, QString afVertiEnd){

    int bytesSent;
    int afHoriStartIntVal;
    int afHoriEndIntVal;
    int afVertiStartIntVal;
    int afVertiEndIntVal;

    if(uvccamera::handle == NULL){
        return void();
    }

    afHoriStartIntVal = afHoriStart.toInt();
    afHoriEndIntVal  = afHoriEnd.toInt();
    afVertiStartIntVal = afVertiStart.toInt();
    afVertiEndIntVal = afVertiEnd.toInt();

    if(afHoriStartIntVal > afHoriEndIntVal || afVertiStartIntVal > afVertiEndIntVal){
        emit deviceStatus("Error", "Horizontal/Vertical AF window start position must be less than or equal to Horizontal/Vertical AF window end postion");
        return void();
    }

    memset(g_out_packet_buf, 0x00, ASCELLA_BUFLEN);

    g_out_packet_buf[1] = 0xFF;
    g_out_packet_buf[2] = 0x02;
    g_out_packet_buf[3] = (afHoriStartIntVal >> 8);
    g_out_packet_buf[4] = (afHoriStartIntVal >> 0);
    g_out_packet_buf[5] = (afHoriEndIntVal >> 8);
    g_out_packet_buf[6] = (afHoriEndIntVal >> 0);
    g_out_packet_buf[7] = (afVertiStartIntVal >> 8);
    g_out_packet_buf[8] = (afVertiStartIntVal >> 0);
    g_out_packet_buf[9] = (afVertiEndIntVal >> 8);
    g_out_packet_buf[10] = (afVertiEndIntVal >> 0);

    qDebug()<< "setcustomweighted af";
    qDebug()<< "afHoriStartIntVal"<<afHoriStartIntVal;
    qDebug()<< "afHoriEndIntVal"<<afHoriEndIntVal;
    qDebug()<< "afVertiStartIntVal"<<afVertiStartIntVal;
    qDebug()<< "afVertiStartIntVal"<<afVertiEndIntVal;
    qDebug()<< "g_out_packet_buf[3]"<<g_out_packet_buf[3];
    qDebug()<< "g_out_packet_buf[4]"<<g_out_packet_buf[4];
    qDebug()<< "g_out_packet_buf[5]"<<g_out_packet_buf[5];
    qDebug()<< "g_out_packet_buf[6]"<<g_out_packet_buf[6];
    qDebug()<< "g_out_packet_buf[7]"<<g_out_packet_buf[7];
    qDebug()<< "g_out_packet_buf[8]"<<g_out_packet_buf[8];
    qDebug()<< "g_out_packet_buf[9]"<<g_out_packet_buf[9];
    qDebug()<< "g_out_packet_buf[10]"<<g_out_packet_buf[10];


    bytesSent = libusb_control_transfer(uvccamera::handle,
                                        0x21,
                                        0x09,
                                        0x200,
                                        0x2,
                                        g_out_packet_buf,
                                        ASCELLA_BUFLEN,
                                        ASCELLA_TIMEOUT);
    qDebug()<<"setcustomweightedaf:bytesSent"<<bytesSent;
    if(0 > bytesSent){
        return void();
    }
}


void ASCELLA::setBinnedResizedMode(camBinnResizeMode mode){

    int bytesSent;

    if(uvccamera::handle == NULL){
        return void();
    }

    if(mode == Binned || mode == Resized){
        memset(g_out_packet_buf, 0x00, ASCELLA_BUFLEN);

        g_out_packet_buf[1] = 0xEE;
        g_out_packet_buf[2] = mode;

        qDebug()<< "setBinnedResized:g_out_packet_buf[2]" << g_out_packet_buf[2];
        bytesSent = libusb_control_transfer(uvccamera::handle,
                                            0x21,
                                            0x09,
                                            0x200,
                                            0x2,
                                            g_out_packet_buf,
                                            ASCELLA_BUFLEN,
                                            ASCELLA_TIMEOUT);
        qDebug()<<"setBinnedResizedMode:bytesSent"<<bytesSent;
        if(0 > bytesSent){
            return void();
        }
    }
    else{
        return void();
    }

}


void ASCELLA::setDefaultValues(){
    u_int8_t defaultValue[30];

    memset(defaultValue, 0x00, sizeof(defaultValue));
    /* get default values */
    getDefaultValues(defaultValue);
    qDebug()<<"here";

    /* enable led off */
    emit ledOffEnable();

    /*exposure compensation is enabled if auto exposure is selected */
    QString exposureValue = QString::number(defaultValue[2]);
    emit autoExposureEnable(exposureValue);

    /*auto focus mode control is enabled if auto focus is checked in Image Quality Settings */


}

void ASCELLA::getDefaultValues(u_int8_t *pDefaultValue){

    int bytesSent;

    if(uvccamera::handle == NULL){
        return void();
    }

    memset(g_out_packet_buf, 0x00, ASCELLA_BUFLEN);
    memset(g_in_packet_buf, 0x00, ASCELLA_BUFLEN);

    g_out_packet_buf[1] = 0xCC;
    g_out_packet_buf[2] = 1;

    // Sending the request command to get default values - output buffer
    qDebug()<< "setDefaultvalue:" << g_out_packet_buf[2];
    bytesSent = libusb_control_transfer(uvccamera::handle,
                                        0x21,
                                        0x09,
                                        0x200,
                                        0x2,
                                        g_out_packet_buf,
                                        ASCELLA_BUFLEN,
                                        ASCELLA_TIMEOUT);
    qDebug()<<"setDefaultValue:bytesSent"<<bytesSent;
    if(0 > bytesSent){
        qDebug()<<"set command failed";
        return void();
    }
    qDebug()<<"Before sending libusb_control_transfer get ";
    // Getting the response - in buffer
//    bytesSent = libusb_control_transfer(uvccamera::handle,
//                                        0x21,
//                                        0x01,
//                                        0x200,
//                                        0x2,
//                                        g_in_packet_buf,
//                                        ASCELLA_BUFLEN,
//                                        ASCELLA_TIMEOUT);

    pDefaultValue[0] = 0x01; //0 - 0x00 //Torch Status: 0 for OFF, 1 for Auto ON, 2 for Manual ON
    pDefaultValue[1] = 0x01; //1 - 0x01 //Torch level: 0-100 supported
    pDefaultValue[2] = 0x06; //2 - 0x06 //Exposure Compensation level: 0-12 supported
    pDefaultValue[3] = 0x03; //3 - 0x03 //AF mode: 0 for one-shot AF, 3 for continuous
    pDefaultValue[4] = 0x01; //4 - 0x00 //Color Mode: 0 for Normal, 1 for Mono, 3 for Negative, 10 for B&W
    pDefaultValue[5] = 0x00; //5 - 0x00 //B&W Threshold: 0 for Auto, 1-255 for Fix Threshold value
    pDefaultValue[6] = 0x00; //6 - 0x00 //bit 7: 0 for Auto, 1 for Fix level; bit 0-6: 1-10 supported
    pDefaultValue[7] = 0x01; //7 - 0x00 //Scene mode: 00 for normal, 0x20 for Document Scanner
    pDefaultValue[8] = 0x01; //8 - 0x00 //Max Frame Rate Control: 0 for Disabling MAx Frame Rate Control, 3-119 for Fixing the frame rate
    pDefaultValue[9] = 0x01;  //9 - 0x01 //AF mode: 1 for center weighted, 2 for custom weighted
    for(int i = 10;i < 18;i++)
        pDefaultValue[i] = 0x00; // 10 to 17 - 0x00 // Auto focus area horizontal and vertical position
    pDefaultValue[18] = 0x01; //18 - 0x01 //B&W: Previous B&W Fix Threshold Value
    pDefaultValue[19] = 0x00;//g_in_packet_buf[21]; //19 - Enable binning/resize; 0 for disable, 1 for enable
    pDefaultValue[20] = 0x01;//g_in_packet_buf[22]; //20 - 1 for binning, 2 for resize
}
