/*
 * see3cam_cu31.cpp -- Handling special feature of SEE3CAM_CU31 camera
 * Copyright © 2024  e-con Systems India Pvt. Limited
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
#include "see3cam_cu31.h"

SEE3CAM_CU31::SEE3CAM_CU31()
{

}



SEE3CAM_CU31::~SEE3CAM_CU31()
{

}


/*
 * @brief SEE3CAM_CU31::setOrientation - Setting orientation - set Normal/horizontal/vertical/Rotate180
 * @param - horizontal flip selection
 * @param - vertical flip selection
 * return true/false
*/
bool SEE3CAM_CU31::setOrientation(bool horzModeSel, bool vertiModeSel)
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_ID_1_SEE3CAM_CU31; /* camera id_1 */
    g_out_packet_buf[2] = CAMERA_CONTROL_ID_2_SEE3CAM_CU31; /* camera id_2 */
    g_out_packet_buf[3] = SET_ORIENTATION_SEE3CAM_CU31; /* set orientation command  */

    if(horzModeSel && vertiModeSel){
        g_out_packet_buf[4] = ROTATE_180; /* both flip enable */
    }else if(horzModeSel && !vertiModeSel){
        g_out_packet_buf[4] = HORIZONTAL; /* horizontal flip only mode */
    }else if(!horzModeSel && vertiModeSel){
        g_out_packet_buf[4] = VERTICAL; /* vertical flip only mode */
    }else
        g_out_packet_buf[4] = NORMAL; /* both flip disable */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6] == SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_ID_1_SEE3CAM_CU31 &&
            g_in_packet_buf[1] == CAMERA_CONTROL_ID_2_SEE3CAM_CU31 &&
            g_in_packet_buf[2] == SET_ORIENTATION_SEE3CAM_CU31 &&
            g_in_packet_buf[6] == SET_SUCCESS) {
            return true;
        }
    }
    return false;
}


/*
 * @brief SEE3CAM_CU31::getOrientation - getting flip mode from the camera
 * return true - success /false - failure
*/
bool SEE3CAM_CU31::getOrientation()
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_ID_1_SEE3CAM_CU31; /* camera id_1 */
    g_out_packet_buf[2] = CAMERA_CONTROL_ID_2_SEE3CAM_CU31; /* camera id_2 */
    g_out_packet_buf[3] = GET_ORIENTATION_SEE3CAM_CU31; /* get orientation command */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6] == GET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_ID_1_SEE3CAM_CU31 &&
            g_in_packet_buf[1] == CAMERA_CONTROL_ID_2_SEE3CAM_CU31 &&
            g_in_packet_buf[2] == GET_ORIENTATION_SEE3CAM_CU31 &&
            g_in_packet_buf[6] == GET_SUCCESS) {
            emit flipMirrorModeChanged(g_in_packet_buf[3]);
            return true;
        }
    }
    return false;
}



/**
 * @brief SEE3CAM_CU31::setRawNoiseReductionStatus - Enable/Disable Raw Noise Reduction in the camera
 * @param status - Mode to set (Enable / Disable)
 * @return true/false
 */
bool SEE3CAM_CU31::setRawNoiseReductionStatus(RNR_STATUS  status)
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_ID_1_SEE3CAM_CU31;
    g_out_packet_buf[2] = CAMERA_CONTROL_ID_2_SEE3CAM_CU31;
    g_out_packet_buf[3] = SET_RNR_STATUS_SEE3CAM_CU31;
    g_out_packet_buf[4] = status;

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6] == SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_ID_1_SEE3CAM_CU31 &&
            g_in_packet_buf[1] == CAMERA_CONTROL_ID_2_SEE3CAM_CU31 &&
            g_in_packet_buf[2] == SET_RNR_STATUS_SEE3CAM_CU31 &&
            g_in_packet_buf[6] == SET_SUCCESS) {
            return true;
        }
    }
    return false;
}


/**
 * @brief SEE3CAM_CU31::getRawNoiseReductionStatus - To get RNR status
 * @return true/false
 */
bool SEE3CAM_CU31::getRawNoiseReductionStatus()
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_ID_1_SEE3CAM_CU31;
    g_out_packet_buf[2] = CAMERA_CONTROL_ID_2_SEE3CAM_CU31;
    g_out_packet_buf[3] = GET_RNR_STATUS_SEE3CAM_CU31;

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6] == GET_FAIL)
        {
            return false;
        }
        else if((g_in_packet_buf[0] == CAMERA_CONTROL_ID_1_SEE3CAM_CU31) &&
            (g_in_packet_buf[1] == CAMERA_CONTROL_ID_2_SEE3CAM_CU31) &&
            (g_in_packet_buf[2] == GET_RNR_STATUS_SEE3CAM_CU31) &&
            (g_in_packet_buf[6] == GET_SUCCESS)){
            emit currentRawNoiseDeductionStatus(g_in_packet_buf[3]);
            return true;
        }
    }
    return false;
}



/**
 * @brief SEE3CAM_CU31::setTriggerMode - To set the Trigger mode to the camera
 * @param mode - Disable/ ReadOut/ Shutter modes
 * @return true/false
 */
bool SEE3CAM_CU31::setTriggerMode(TRIGGER_MODE  mode)
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_ID_1_SEE3CAM_CU31;
    g_out_packet_buf[2] = CAMERA_CONTROL_ID_2_SEE3CAM_CU31;
    g_out_packet_buf[3] = SET_TRIGGER_MODE_SEE3CAM_CU31;
    g_out_packet_buf[4] = mode;

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6] == SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_ID_1_SEE3CAM_CU31 &&
            g_in_packet_buf[1] == CAMERA_CONTROL_ID_2_SEE3CAM_CU31 &&
            g_in_packet_buf[2] == SET_TRIGGER_MODE_SEE3CAM_CU31 &&
            g_in_packet_buf[6] == SET_SUCCESS) {
            return true;
        }
    }
    return false;
}


/**
 * @brief SEE3CAM_CU31::getTriggerMode - To get trigger mode from the camera
 * @return true/false
 */
bool SEE3CAM_CU31::getTriggerMode()
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_ID_1_SEE3CAM_CU31;
    g_out_packet_buf[2] = CAMERA_CONTROL_ID_2_SEE3CAM_CU31;
    g_out_packet_buf[3] = GET_TRIGGER_MODE_SEE3CAM_CU31;

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6] == GET_FAIL)
        {
            return false;
        }
        else if((g_in_packet_buf[0] == CAMERA_CONTROL_ID_1_SEE3CAM_CU31) &&
            (g_in_packet_buf[1] == CAMERA_CONTROL_ID_2_SEE3CAM_CU31) &&
            (g_in_packet_buf[2] == GET_TRIGGER_MODE_SEE3CAM_CU31) &&
            (g_in_packet_buf[6] == GET_SUCCESS)){
            emit currentTriggerMode(g_in_packet_buf[3]);
            return true;
        }
    }
    return false;
}



/**
 * @brief SEE3CAM_CU31::setAEConvergenceSpeed - To set Convergence speed to the camera
 * @param convergenceSpeed - The value to be set
 * @return true/false
 */
bool SEE3CAM_CU31::setAEConvergenceSpeed(uint convergenceSpeed)
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_ID_1_SEE3CAM_CU31;
    g_out_packet_buf[2] = CAMERA_CONTROL_ID_2_SEE3CAM_CU31;
    g_out_packet_buf[3] = SET_AE_CONVERGENCE_SPEED_SEE3CAM_CU31;
    g_out_packet_buf[4] = convergenceSpeed;

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6] == SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_ID_1_SEE3CAM_CU31 &&
            g_in_packet_buf[1] == CAMERA_CONTROL_ID_2_SEE3CAM_CU31 &&
            g_in_packet_buf[2] == SET_AE_CONVERGENCE_SPEED_SEE3CAM_CU31 &&
            g_in_packet_buf[6] == SET_SUCCESS) {
            return true;
        }
    }
    return false;
}


/**
 * @brief SEE3CAM_CU31::getAEConvergenceSpeed - To get convergence speed from the camera
 * @return true/false
 */
bool SEE3CAM_CU31::getAEConvergenceSpeed()
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_ID_1_SEE3CAM_CU31;
    g_out_packet_buf[2] = CAMERA_CONTROL_ID_2_SEE3CAM_CU31;
    g_out_packet_buf[3] = GET_AE_CONVERGENCE_SPEED_SEE3CAM_CU31;

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6] == GET_FAIL)
        {
            return false;
        }
        else if((g_in_packet_buf[0] == CAMERA_CONTROL_ID_1_SEE3CAM_CU31) &&
            (g_in_packet_buf[1] == CAMERA_CONTROL_ID_2_SEE3CAM_CU31) &&
            (g_in_packet_buf[2] == GET_AE_CONVERGENCE_SPEED_SEE3CAM_CU31) &&
            (g_in_packet_buf[6] == GET_SUCCESS)){
            emit currentAEConvergenceSpeed(g_in_packet_buf[3]);
            return true;
        }
    }
    return false;
}



/**
 * @brief SEE3CAM_CU31::setAutoExposureStatus
 * @param exposureStatus - The value to be set
 * @return true/false
 */
bool SEE3CAM_CU31::setAutoExposureStatus(AUTO_EXPOSURE exposureStatus)
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_ID_1_SEE3CAM_CU31;
    g_out_packet_buf[2] = CAMERA_CONTROL_ID_2_SEE3CAM_CU31;
    g_out_packet_buf[3] = SET_AE_HOLD_SEE3CAM_CU31;
    g_out_packet_buf[4] = exposureStatus;

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6] == SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_ID_1_SEE3CAM_CU31 &&
            g_in_packet_buf[1] == CAMERA_CONTROL_ID_2_SEE3CAM_CU31 &&
            g_in_packet_buf[2] == SET_AE_HOLD_SEE3CAM_CU31 &&
            g_in_packet_buf[6] == SET_SUCCESS) {
            return true;
        }
    }
    return false;
}


/**
 * @brief SEE3CAM_CU31::getAutoExpStatus
 * @return true/false
 */
bool SEE3CAM_CU31::getAutoExpStatus()
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_ID_1_SEE3CAM_CU31;
    g_out_packet_buf[2] = CAMERA_CONTROL_ID_2_SEE3CAM_CU31;
    g_out_packet_buf[3] = GET_AE_HOLD_SEE3CAM_CU31;

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6] == GET_FAIL) {
            return false;
        }else if((g_in_packet_buf[0] == CAMERA_CONTROL_ID_1_SEE3CAM_CU31) &&
            (g_in_packet_buf[1] == CAMERA_CONTROL_ID_2_SEE3CAM_CU31) &&
            (g_in_packet_buf[2] == GET_AE_HOLD_SEE3CAM_CU31) &&
            (g_in_packet_buf[6] == GET_SUCCESS)){
            emit currentAutoExposureStatus(g_in_packet_buf[3]);
            return true;
        }
    }
    return false;
}




/**
 * @brief SEE3CAM_CU31::setAWBStatus
 * @param exposureStatus - The value to be set
 * @return true/false
 */
bool SEE3CAM_CU31::setAWBStatus(AUTO_WHITE_BALANCE status)
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_ID_1_SEE3CAM_CU31;
    g_out_packet_buf[2] = CAMERA_CONTROL_ID_2_SEE3CAM_CU31;
    g_out_packet_buf[3] = SET_AWB_STATUS_SEE3CAM_CU31;
    g_out_packet_buf[4] = status;

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6] == SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_ID_1_SEE3CAM_CU31 &&
            g_in_packet_buf[1] == CAMERA_CONTROL_ID_2_SEE3CAM_CU31 &&
            g_in_packet_buf[2] == SET_AWB_STATUS_SEE3CAM_CU31 &&
            g_in_packet_buf[6] == SET_SUCCESS) {
            return true;
        }
    }
    return false;
}


/**
 * @brief SEE3CAM_CU31::getAWBStatus
 * @return true/false
 */
bool SEE3CAM_CU31::getAWBStatus()
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_ID_1_SEE3CAM_CU31;
    g_out_packet_buf[2] = CAMERA_CONTROL_ID_2_SEE3CAM_CU31;
    g_out_packet_buf[3] = GET_AWB_STATUS_SEE3CAM_CU31;

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6] == GET_FAIL){
            return false;
        }else if((g_in_packet_buf[0] == CAMERA_CONTROL_ID_1_SEE3CAM_CU31) &&
            (g_in_packet_buf[1] == CAMERA_CONTROL_ID_2_SEE3CAM_CU31) &&
            (g_in_packet_buf[2] == GET_AWB_STATUS_SEE3CAM_CU31) &&
            (g_in_packet_buf[6] == GET_SUCCESS)){
            emit currentAutoWhiteBalanceStatus(g_in_packet_buf[3]);
            return true;
        }
    }
    return false;
}



/**
 * @brief SEE3CAM_CU31::readTeperature() - To read the temperature in the camera
 * @return true/false
 */
bool SEE3CAM_CU31::readTemperature(){
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    float temperature;

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_ID_1_SEE3CAM_CU31;
    g_out_packet_buf[2] = CAMERA_CONTROL_ID_2_SEE3CAM_CU31;
    g_out_packet_buf[3] = READ_TEMPERATURE_SEE3CAM_CU31;

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){

        if (g_in_packet_buf[15] == SET_FAIL) {
            return false;
        } else if((g_in_packet_buf[0] == CAMERA_CONTROL_ID_1_SEE3CAM_CU31) &&
            (g_in_packet_buf[1] == CAMERA_CONTROL_ID_2_SEE3CAM_CU31) &&
            (g_in_packet_buf[2] == READ_TEMPERATURE_SEE3CAM_CU31) &&
            g_in_packet_buf[15] == SET_SUCCESS) {

            uint32_t combinedValue = (g_in_packet_buf[6] << 24) | (g_in_packet_buf[5] << 16) | (g_in_packet_buf[4] << 8) | g_in_packet_buf[3];
            temperature = *reinterpret_cast<float*>(&combinedValue);

            emit currentTemperature(temperature);

            return true;
        }
    }
    return false;
}



/**
 * @brief SEE3CAM_CU31::setToDefaultValues - set all the values to default in camera
 * @return true/false
 */
bool SEE3CAM_CU31::setToDefaultValues()
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_ID_1_SEE3CAM_CU31;
    g_out_packet_buf[2] = CAMERA_CONTROL_ID_2_SEE3CAM_CU31;
    g_out_packet_buf[3] = SET_DEFAULT_SEE3CAM_CU31;

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH))
    {
        if (g_in_packet_buf[6] == SET_FAIL){
            return false;
        }else if(g_in_packet_buf[0] == CAMERA_CONTROL_ID_1_SEE3CAM_CU31 &&
            g_in_packet_buf[1] == CAMERA_CONTROL_ID_2_SEE3CAM_CU31 &&
            g_in_packet_buf[2] == SET_DEFAULT_SEE3CAM_CU31 &&
            g_in_packet_buf[6] == SET_SUCCESS){
            return true;
        }
    }
    return false;
}



/**
 * @brief SEE3CAM_CU31::get64BitSerialNumber - To get 64-bit Serial number
 * @return true/false
 */
bool SEE3CAM_CU31::get64BitSerialNumber()
{
    QString lsb = "";
    QString msb = "";

    bool timeout = true;
    int ret = 0;
    unsigned int start, end = 0;

    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize the buffer
    memset(g_out_packet_buf, 0x00, sizeof(g_out_packet_buf));

    //Set the Report Number
    g_out_packet_buf[1] = GET_64BIT_UNIQUE_ID_1; 	/* Report Number */

    ret = write(uvccamera::hid_fd, g_out_packet_buf, BUFFER_LENGTH);
    if (ret < 0) {
        _text = tr("Device not available");
        return false;
    }
    /* Read the Unique id from the device */
    start = uvc.getTickCount();
    while(timeout)
    {
        /* Get a report from the device */
        ret = read(uvccamera::hid_fd, g_in_packet_buf, BUFFER_LENGTH);
        if (ret < 0)
        {
        }
        else
        {
            if((g_in_packet_buf[0] == GET_64BIT_UNIQUE_ID_1))
            {
                lsb.sprintf("%02x%02x%02x%02x",g_in_packet_buf[1],g_in_packet_buf[2],g_in_packet_buf[3],g_in_packet_buf[4]);

                //Initialize the buffer
                memset(g_out_packet_buf, 0x00, sizeof(g_out_packet_buf));

                //Set the Report Number
                g_out_packet_buf[1] = GET_64BIT_UNIQUE_ID_1;
                g_out_packet_buf[2] = GET_64BIT_UNIQUE_ID_2;

                ret = write(uvccamera::hid_fd, g_out_packet_buf, BUFFER_LENGTH);
                if (ret < 0) {
                    _text = tr("Device not available");
                    return false;
                }
                start = uvc.getTickCount();

                while(timeout)
                {
                    /* Get a report from the device */
                    ret = read(uvccamera::hid_fd, g_in_packet_buf, BUFFER_LENGTH);
                    if (ret < 0)
                    {
                    }
                    else
                    {
                        if((g_in_packet_buf[0] == GET_64BIT_UNIQUE_ID_1))
                        {
                            msb.sprintf("%02x%02x%02x%02x",g_in_packet_buf[2],g_in_packet_buf[3],g_in_packet_buf[4],g_in_packet_buf[5]);
                            timeout = false;
                        }
                    }
                }
            }
        }
        end = uvc.getTickCount();
        if(end - start > TIMEOUT)
        {
            timeout = false;
            return false;
        }
    }
    _text.clear();
    _text.append(lsb+msb);
    _title.clear();
    _title = tr("Serial Number");
    emit titleTextChanged(_title,_text);
    return true;
}


/*
 * @brief SEE3CAM_CU31::initializeBuffers - Initialize input and output buffers
 */
void SEE3CAM_CU31::initializeBuffers(){
    //Initialize input and output buffers
    memset(g_out_packet_buf, 0x00, sizeof(g_out_packet_buf));
    memset(g_in_packet_buf, 0x00, sizeof(g_in_packet_buf));
}
