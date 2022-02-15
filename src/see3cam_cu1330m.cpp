#include "see3cam_cu1330m.h"

See3CAM_CU1330M::See3CAM_CU1330M()
{
}

See3CAM_CU1330M::~See3CAM_CU1330M()
{
}


void See3CAM_CU1330M::initializeBuffers()
{
    //Initialize input and output buffers
    memset(g_out_packet_buf, 0x00, sizeof(g_out_packet_buf));
    memset(g_in_packet_buf, 0x00, sizeof(g_in_packet_buf));
}

/**
 * @brief See3CAM_CU1330M::setStreamMode - Setting  Streaming mode
 * @param streamMode - mode selected in UI
 * @return true/false
 */
bool See3CAM_CU1330M::setStreamMode(streamModes streamMode){
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_CU1330M; /* camera id */
    g_out_packet_buf[2] = SET_STREAM_MODE_CU1330M; /* set stream mode command */
    g_out_packet_buf[3] = streamMode; /* stream mode to set */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_CU1330M &&
            g_in_packet_buf[1]==SET_STREAM_MODE_CU1330M &&
            g_in_packet_buf[6]==SET_SUCCESS) {
            return true;
        }
    }
    return false;
}


/**
 * @brief See3CAM_CU1330M::getStreamMode - get Stream mode from camera
 * return true - success /false - failure
 */
bool See3CAM_CU1330M::getStreamMode()
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_CU1330M; /* camera id */
    g_out_packet_buf[2] = GET_STREAM_MODE_CU1330M; /* get stream mode command */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==GET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_CU1330M &&
            g_in_packet_buf[1]==GET_STREAM_MODE_CU1330M &&
            g_in_packet_buf[6]==GET_SUCCESS) {
            emit streamModeValue(g_in_packet_buf[2]);
            return true;
        }
    }
    return false;
}

bool See3CAM_CU1330M ::setFlashState(flashStateValues flashMode)
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }
    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] =CAMERA_CONTROL_CU1330M; /* set camera control code */
    g_out_packet_buf[2] = SET_FLASH_MODE_CU1330M ; /* set flash mode code */
    g_out_packet_buf[3] = flashMode; /* actual flash mode */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==GET_FAILURE) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_CU1330M &&
            g_in_packet_buf[1]==SET_FLASH_MODE_CU1330M  &&
            g_in_packet_buf[6]==SET_SUCCESS) {
            return true;
        }
    }

    return false;
}


bool See3CAM_CU1330M::getFlashState()
{
   // hid validation
   if(uvccamera::hid_fd < 0)
   {
       return false;
   }

   //Initialize buffers
   initializeBuffers();

   // fill buffer values
   g_out_packet_buf[1] = CAMERA_CONTROL_CU1330M;  /* set camera control code */
   g_out_packet_buf[2] = GET_FLASH_MODE_CU1330M ; /* get flash mode code */

   // send request and get reply from camera
   if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
       if (g_in_packet_buf[6]==GET_FAILURE) {
           return false;
       } else if(g_in_packet_buf[0] == CAMERA_CONTROL_CU1330M &&
           g_in_packet_buf[1]==GET_FLASH_MODE_CU1330M  &&
           g_in_packet_buf[6]==GET_SUCCESS) {
           emit flashModeValue(g_in_packet_buf[2]);
           return true;
       }
   }
   return false;
}


bool See3CAM_CU1330M::setFlipCtrlValue(bool hFlip ,bool vFlip)
{
    u_int8_t flipMode;

    if(hFlip && vFlip){
        flipMode = FLIP_BOTH; /* both flip enable */
    }else if(hFlip && !vFlip){
        flipMode = FLIP_HORIZONTAL; /* horizontal flip only mode */
    }else if(!hFlip && vFlip){
        flipMode = FLIP_VERTICAL; /* vertical flip only mode */
    }else{
        flipMode = FLIP_NORMAL;
    }
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_CU1330M; /* set camera control code */
    g_out_packet_buf[2] = SET_ORIENTATION_CU1330M ; /* set flip control code */
    g_out_packet_buf[3] = flipMode; /* set flipControl value */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_CU1330M &&
            g_in_packet_buf[1]==SET_ORIENTATION_CU1330M  &&
            g_in_packet_buf[6]==SET_SUCCESS) {
            return true;
        }
    }
    return false;
}


/**
 * @brief See3CAM_CU1330M::getFlipCtrlValue - get flip control value from camera
 * return true - success /false - failure
 */
bool See3CAM_CU1330M::getFlipCtrlValue()
{
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_CU1330M; /* set camera control code */
    g_out_packet_buf[2] = GET_ORIENTATION_CU1330M ; /* get flip control code */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==GET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_CU1330M &&
            g_in_packet_buf[1]==GET_ORIENTATION_CU1330M  &&
            g_in_packet_buf[6]==GET_SUCCESS) {
            emit flipCtrlValue(g_in_packet_buf[2]);
            return true;
        }
    }
    return false;
}

/**
 * @brief See3CAM_CU1330M::setToDefault - set all the values to default in camera
 * @return true/false
 */
bool See3CAM_CU1330M::setToDefault(){

    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_CU1330M; /* camera id */
    g_out_packet_buf[2] = SET_TO_DEFAULT_CU1330M; /* set to default command */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_CU1330M &&
            g_in_packet_buf[1]==SET_TO_DEFAULT_CU1330M &&
            g_in_packet_buf[6]==SET_SUCCESS) {
            return true;
        }
    }
    return false;
}

/**
 * @brief See3CAM_CU1330M::setROIAutoExposure - Set ROI auto exposure to camera
 * @param camROIAutoExposureMode - ROI mode
 * @param vidResolnWidth - preview resolution width
 * @param vidResolnHeight - preview resolution height
 * @param xCord - mouse xCordinate from preview
 * @param yCord - mouse yCordinate from preview
 * @param winSize - ROI window size
 * return true - success /false - failure
 */
bool See3CAM_CU1330M::setROIAutoExposure(camROIAutoExpMode see3camAutoexpROIMode, uint vidResolnWidth, uint vidResolnHeight, uint xCord, uint yCord, QString winSize)
{

    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //((Input - InputLow) / (InputHigh - InputLow)) * (OutputHigh - OutputLow) + OutputLow // map resolution width and height -  0 to 255

    double outputLow = 0;
    double outputHigh = 255;
    double inputXLow = 0;
    double inputXHigh = vidResolnWidth-1;
    double inputXCord = xCord;
    int outputXCord = ((inputXCord - inputXLow) / (inputXHigh - inputXLow)) * (outputHigh - outputLow) + outputLow;

    double inputYLow = 0;
    double inputYHigh = vidResolnHeight-1;
    double inputYCord = yCord;
    int outputYCord = ((inputYCord - inputYLow) / (inputYHigh - inputYLow)) * (outputHigh - outputLow) + outputLow;

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_CU1330M; /* camera id */
    g_out_packet_buf[2] = SET_EXP_ROI_MODE_CU1330M; /* set exposure ROI command */
    g_out_packet_buf[3] = see3camAutoexpROIMode; /* exposure ROI mode to set */

    if(see3camAutoexpROIMode == AutoExpManual){
        g_out_packet_buf[4] = outputXCord; // x cord
        g_out_packet_buf[5] = outputYCord; // y cord
        g_out_packet_buf[6] = winSize.toUInt(); // window size
    }

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==SET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_CU1330M &&
            g_in_packet_buf[1]==SET_EXP_ROI_MODE_CU1330M &&
            g_in_packet_buf[6]==SET_SUCCESS) {
            return true;
        }
    }
    return false;
}


/**
 * @brief See3CAM_CU1330M::getAutoExpROIModeAndWindowSize - get ROI auto exposure mode and window size
 * return true - success /false - failure
 */
bool See3CAM_CU1330M::getAutoExpROIModeAndWindowSize(){
    // hid validation
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    //Initialize buffers
    initializeBuffers();

    // fill buffer values
    g_out_packet_buf[1] = CAMERA_CONTROL_CU1330M; /* camera id */
    g_out_packet_buf[2] = GET_EXP_ROI_MODE_CU1330M; /* get exposure ROI mode  */

    // send request and get reply from camera
    if(uvc.sendHidCmd(g_out_packet_buf, g_in_packet_buf, BUFFER_LENGTH)){
        if (g_in_packet_buf[6]==GET_FAIL) {
            return false;
        } else if(g_in_packet_buf[0] == CAMERA_CONTROL_CU1330M &&
            g_in_packet_buf[1]==GET_EXP_ROI_MODE_CU1330M &&
            g_in_packet_buf[6]==GET_SUCCESS) {
            emit roiAutoExpMode(g_in_packet_buf[2], g_in_packet_buf[5]);
            return true;
        }
    }
    return false;
}
