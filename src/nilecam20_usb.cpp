#include<nilecam20_usb.h>


/**
 * @brief See3CAM_CU20::setSensorMode - setting sensor mode
 * @param sensorMode - standard/dsr hlo mode
 * @return true/false
 */
bool NILECAM20_USB::setSensorMode(sensorModes sensorMode)
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* camera id */
    g_out_packet_buf[2] = SET_SENSOR_MODE_NILECAM20_USB; /* set sensor command  */
    g_out_packet_buf[3] = sensorMode; /* pass camera mode value */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1]==SET_SENSOR_MODE_NILECAM20_USB &&
            g_in_packet_buf[6]==SET_SUCCESS) {\
            return true;
        }
    }
    return false;
}

/**
 * @brief See3CAM_CU20::setSensorMode - setting sensor mode
 * @return true/false
 */
bool NILECAM20_USB::getSensorMode()
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* camera id */
    g_out_packet_buf[2] = GET_SENSOR_MODE_NILECAM20_USB; /* get sensor command  */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==GET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1]==GET_SENSOR_MODE_NILECAM20_USB &&
            g_in_packet_buf[6]==GET_SUCCESS) {
            emit sensorModeReceived(g_in_packet_buf[2]);
            return true;
        }
    }
    return false;
}


/**
 * @brief NILECAM20_USB::setCameraMode - setting camera mode
 * @param cameraMode - master/slave
 * @return true/false
 */
bool NILECAM20_USB::setCameraMode(cameraModes  cameraMode)
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* camera id */
    g_out_packet_buf[2] = SET_CAMERA_MODE_NILECAM20_USB; /* set camera mode command  */
    g_out_packet_buf[3] = cameraMode; /* pass camera mode value */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1] == SET_CAMERA_MODE_NILECAM20_USB &&
            g_in_packet_buf[6] == SET_SUCCESS) {
            return true;
        }
    }
    return false;
}

/**
 * @brief See3CAM_CU22::setCameraMode - getting camera mode
 * @return true/false
 */
bool NILECAM20_USB::getCameraMode()
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* camera id */
    g_out_packet_buf[2] = GET_CAMERA_MODE_NILECAM20_USB; /* get camera command  */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==GET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1]==GET_CAMERA_MODE_NILECAM20_USB &&
            g_in_packet_buf[6]==GET_SUCCESS) {
            emit cameraModeReceived(g_in_packet_buf[2]);
            return true;
        }
    }
    return false;
}

/**
 * @brief NILECAM20_USB::getSpecialEffectsNileCam - get effect mode set in camera
 * return true - success /false - failure
 */
bool NILECAM20_USB::getSpecialEffectsNileCam20USB()
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* set camera control code */
    g_out_packet_buf[2] = GET_SPECIAL_MODE_NILECAM20_USB; /* get special effect code */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==GET_FAIL)
        {
            return false;
        }
        else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1]==GET_SPECIAL_MODE_NILECAM20_USB &&
            g_in_packet_buf[6]==GET_SUCCESS)
        {
            emit sendEffectMode(g_in_packet_buf[2]);
            return true;
        }
    }
    return false;
}


/**
 * @brief NILECAM20_USB::setSpecialEffectsNileCam - setting special effects to the camera
 * return true - success /false - failure
 */
bool NILECAM20_USB::setSpecialEffectsNileCam20USB(const specialEffects &specialEffect)
{

    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* set camera control code */
    g_out_packet_buf[2] = SET_SPECIAL_MODE_NILECAM20_USB; /* set special effect code */
    g_out_packet_buf[3] = specialEffect; /* set special effect */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1]==SET_SPECIAL_MODE_NILECAM20_USB &&
            g_in_packet_buf[6]==SET_SUCCESS) {
            return true;
        }
    }
    return false;
}

/*
 * @brief See3CAM_CU130::setOrientation - Setting orientation - set horizontal/vertical/both flip
 * @param - horizontal flip selection
 * @param - vertical flip selection
 * return true/false
*/
bool NILECAM20_USB::setOrientation(bool horzModeSel, bool vertiModeSel)
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* camera control for see3cam_30 camera */
    g_out_packet_buf[2] = SET_ORIENTATION_NILECAM20_USB; /* set flip mode for 30 camera */
    if(horzModeSel && vertiModeSel){
        g_out_packet_buf[3] = NORMAL_NILECAM20_USB; /* both flip enable */
    }else if(horzModeSel && !vertiModeSel){
        g_out_packet_buf[3] = HORIZONTAL_MIRROR_NILECAM20_USB; /* horizontal flip only mode */
    }else if(!horzModeSel && vertiModeSel){
        g_out_packet_buf[3] = VERTICAL_FLIP_NILECAM20_USB; /* vertical flip only mode */
    }else
        g_out_packet_buf[3] = ROTATE_NILECAM20_USB; /* both flip disable */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1]==SET_ORIENTATION_NILECAM20_USB &&
            g_in_packet_buf[6]==SET_SUCCESS) {
            return true;
        }
    }
    return false;
}

/**
 * @brief See3CAM_CU130::getOrientation - getting flip mode from the camera
 * return true - success /false - failure
 */
bool NILECAM20_USB::getOrientation()
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* Camera control id */
    g_out_packet_buf[2] = GET_ORIENTATION_NILECAM20_USB; /* get orientation command */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1]==GET_ORIENTATION_NILECAM20_USB &&
            g_in_packet_buf[6]==GET_SUCCESS) {
            emit flipMirrorModeChanged(g_in_packet_buf[2]);
            return true;
        }
    }
    return false;
}

bool NILECAM20_USB::setStrobeMode(strobeValues strobeMode){
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* camera id */
    g_out_packet_buf[2] = SET_STROBE_MODE_NILECAM20_USB; /* set strobe mode command  */
    g_out_packet_buf[3] = strobeMode; /* pass strobe mode value */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1] == SET_STROBE_MODE_NILECAM20_USB &&
            g_in_packet_buf[6] == SET_SUCCESS) {
            return true;
        }
    }
    return false;
}

/**
 * @brief See3CAM_CU20::getStrobeMode - get Strobe mode
 * @return true/false
 */
bool NILECAM20_USB::getStrobeMode(){

    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* camera id */
    g_out_packet_buf[2] = GET_STROBE_MODE_NILECAM20_USB; /* get strobe mode command  */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==GET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1] == GET_STROBE_MODE_NILECAM20_USB &&
            g_in_packet_buf[6] == GET_SUCCESS) {
            emit strobeModeChanged(g_in_packet_buf[2]);
            return true;
        }
    }
    return false;
}




//bool NILECAM20_USB::setColorKillModeNileCam20USB(int deNoiseVal)
//{
//    // hid validation
//    if(uvccamera::hid_fd < 0)
//    {
//        return false;
//    }

//    //Initialize buffers
//    initializeBuffers();

//    // fill buffer values
//    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* set camera control code */
//    g_out_packet_buf[2] = SET_DENOISE_NILECAM20_USB; /* set denoise control code */
//    g_out_packet_buf[3] = deNoiseVal; /* set denoise value */

//    // send request and get reply from camera
//    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
//        if (g_in_packet_buf[6]==SET_FAIL) {
//            return false;
//        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
//            g_in_packet_buf[1]==SET_DENOISE_NILECAM20_USB &&
//            g_in_packet_buf[6]==SET_SUCCESS) {
//            return true;
//        }
//    }
//    return false;
//}

/**
 * @brief NILECAM20_USB::setDenoiseValueNileCam - setting denoise value to the camera
 * @param deNoiseVal - denoise value
 * return true - success /false - failure
 */
bool NILECAM20_USB::setDenoiseValueNileCam20USB(int deNoiseVal)
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* set camera control code */
    g_out_packet_buf[2] = SET_DENOISE_NILECAM20_USB; /* set denoise control code */
    g_out_packet_buf[3] = deNoiseVal; /* set denoise value */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1]==SET_DENOISE_NILECAM20_USB &&
            g_in_packet_buf[6]==SET_SUCCESS) {
            return true;
        }
    }
    return false;
}


/**
 * @brief NILECAM30_USB::getDenoiseValueNileCam - get denoise value from camera
 * return true - success /false - failure
 */
bool NILECAM20_USB::getDenoiseValueNileCam30USB()
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* set camera control code */
    g_out_packet_buf[2] = GET_DENOISE_NILECAM20_USB; /* get denoise code */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==GET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1]==GET_DENOISE_NILECAM20_USB &&
            g_in_packet_buf[6]==GET_SUCCESS) {
            emit sendDenoiseValue(g_in_packet_buf[2]);
            return true;
        }
    }
    return false;
}


/**
 * @brief NILECAM30_USB::setExposureCompensationNileCam - setting exposure compensation
 * @param exposureCompValue - exposure compensation value
 * @return true/false
 */
bool NILECAM20_USB::setExposureCompensationNileCam30USB(unsigned int exposureCompValue){
    if(EXPOSURECOMP_MIN > exposureCompValue || EXPOSURECOMP_MAX < exposureCompValue){
        emit indicateExposureValueRangeFailure("Failure", "Given exposure compensation value is invalid");
        return false;
    }

    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* camera id */
    g_out_packet_buf[2] = SET_EXPOSURE_COMPENSATION_NILECAM20_USB; /* set exposure compensation command */
    g_out_packet_buf[3] = (u_int8_t)((exposureCompValue & 0xFF000000) >> 24);
    g_out_packet_buf[4] = (u_int8_t)((exposureCompValue & 0x00FF0000) >> 16);
    g_out_packet_buf[5] = (u_int8_t)((exposureCompValue & 0x0000FF00) >> 8);
    g_out_packet_buf[6] = (u_int8_t)((exposureCompValue & 0x000000FF) >> 0);

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==SET_FAIL) {
            emit indicateCommandStatus("Failure", "Failed to set exposure compensation value");
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1]==SET_EXPOSURE_COMPENSATION_NILECAM20_USB && g_in_packet_buf[6]==SET_SUCCESS) {\
            emit indicateCommandStatus("Success", "Exposure compensation value is set successfully");
            return true;
        }
    }
    return false;
}


/**
 * @brief NILECAM30_USB::getExposureCompensationNileCam - getting exposure compensation
 * @return true/false
 */
bool NILECAM20_USB::getExposureCompensationNileCam30USB(){
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* set camera control code */
    g_out_packet_buf[2] = GET_EXPOSURE_COMPENSATION_NILECAM20_USB; /* get exposure compensation code */

    unsigned int exposureCompValue;

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==GET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1]== GET_EXPOSURE_COMPENSATION_NILECAM20_USB && g_in_packet_buf[6]==GET_SUCCESS) {\
            exposureCompValue = (((u_int8_t)g_in_packet_buf[2]) << 24)
                                | (((u_int8_t)g_in_packet_buf[3]) << 16)
                                | (((u_int8_t)g_in_packet_buf[4]) << 8)
                                | (((u_int8_t)g_in_packet_buf[5]) << 0);
            emit exposureCompValueReceived(exposureCompValue);
            return true;
        }
    }
    return false;
}

/**
 * @brief NILECAM30_USB::restoreDefaultNileCam - set all the values to default in camera
 * @return true/false
 */
bool NILECAM20_USB::restoreDefaultNileCam30USB(){

    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_NILECAM20_USB; /* camera id */
    g_out_packet_buf[2] = SET_DEFAULT_NILECAM20_USB; /* set to default command */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_NILECAM20_USB &&
            g_in_packet_buf[1]==SET_DEFAULT_NILECAM20_USB &&
            g_in_packet_buf[6]==SET_SUCCESS) {\
            return true;
        }
    }
    return false;
}

/**
 * @brief NILECAM30_USB::initializeBuffers - Initialize input and output buffers
 */
void NILECAM20_USB::initializeBuffers(){

    //Initialize input and output buffers
    memset(g_out_packet_buf, 0x00, sizeof(g_out_packet_buf));
    memset(g_in_packet_buf, 0x00, sizeof(g_in_packet_buf));
}

