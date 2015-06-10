
#include "seecam_ar0130.h"

bool See3CAM_AR130::enableMasterMode()
{
    bool ret = modeControl.enableMasterMode();
    return ret;
}

bool  See3CAM_AR130::enableTriggerMode()
{
    bool ret = modeControl.enableTriggerMode();
    return ret;
}

void See3CAM_AR130::setCroppedVGAMode()
{
    bool ret = false;
    u_int8_t cropped_vga_status = 0;
    ret = enableCroppedVGAMode(&cropped_vga_status);
    if(!ret)
    {
        printf("Unable to switch to cropped VGA Mode");
        emit deviceStatus("Failure", "Unable to switch to cropped VGA Mode");
        return void();
    }
    else
    {
        switch(cropped_vga_status)
        {
        case 1 :
            printf("Cropped VGA mode set successfully\n");
            emit deviceStatus("Success","Cropped VGA mode set successfully");
            break;
        case 2 :
            printf("The current resolution is not 640x480, please switch to 640x480 before using the Cropping and Binning modes");
            emit deviceStatus("Failure","The current resolution is not 640x480, please switch to 640x480 before using the Cropping and Binning modes");
            break;
        case 3 :
            printf("Device is already in Cropped VGA mode");
            emit deviceStatus("Failure","Device is already in Cropped VGA mode");
            break;
        case 4 :
            printf("Failed to set Cropped VGA mode");
            emit deviceStatus("Failure","Failed to set Cropped VGA mode");
            break;
        default :
            printf("Unknown %d\n ",cropped_vga_status);
            emit deviceStatus("Failure","Unknown error");

        }
    }
}

bool See3CAM_AR130::enableCroppedVGAMode(u_int8_t *VGAStatus)
{
    *VGAStatus = 0;
    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    bool timeout = true;
    int ret =0;
    unsigned int start, end = 0;

    //Initialize the buffer
    memset(g_out_packet_buf, 0x00, sizeof(g_out_packet_buf));

    //Set the Report Number
    g_out_packet_buf[1] = ENABLE_CROPPED_VGA_MODE; /* Report Number */

    ret = write(uvccamera::hid_fd, g_out_packet_buf, BUFFER_LENGTH);
    if (ret < 0) {
        perror("write");
        return false;
    } else {
        printf("%s(): write() wrote %d bytes\n", __func__, ret);
    }
    start = uvc.getTickCount();
    while(timeout)
    {
        /* Get a report from the device */
        ret = read(uvccamera::hid_fd, g_in_packet_buf, BUFFER_LENGTH);
        if (ret < 0) {
            //perror("read");
        } else {
            printf("%s(): read %d bytes:\n", __func__,ret);
            if(g_in_packet_buf[0] == ENABLE_CROPPED_VGA_MODE) {
                *VGAStatus = g_in_packet_buf[1];
                timeout = false;
            }
        }
        end = uvc.getTickCount();
        if(end - start > TIMEOUT)
        {
            printf("%s(): Timeout occurred\n", __func__);
            timeout = false;
            return false;
        }
    }
    return true;
}

bool See3CAM_AR130::enableBinnedVGAMode(u_int8_t *VGAStatus)
{
    *VGAStatus = 0;

    if(uvccamera::hid_fd < 0)
    {
        return false;
    }

    bool timeout = true;
    int ret = 0;
    unsigned int start, end = 0;

    //Initialize the buffer
    memset(g_out_packet_buf, 0x00, sizeof(g_out_packet_buf));

    //Set the Report Number
    g_out_packet_buf[1] = ENABLE_BINNED_VGA_MODE; /* Report Number */

    ret = write(uvccamera::hid_fd, g_out_packet_buf, BUFFER_LENGTH);
    if (ret < 0) {
        perror("write");
        return false;
    } else {
        printf("%s(): write() wrote %d bytes\n", __func__, ret);
    }
    start = uvc.getTickCount();
    while(timeout)
    {
        /* Get a report from the device */
        ret = read(uvccamera::hid_fd, g_in_packet_buf, BUFFER_LENGTH);
        if (ret < 0) {
            //perror("read");
        } else {
            printf("%s(): read %d bytes:\n", __func__,ret);
            if(g_in_packet_buf[0] == ENABLE_BINNED_VGA_MODE) {
                *VGAStatus = g_in_packet_buf[1];
                timeout = false;
            }
        }
        end = uvc.getTickCount();
        if(end - start > TIMEOUT)
        {
            printf("%s(): Timeout occurred\n", __func__);
            timeout = false;
            return false;
        }
    }

    return true;
}

void See3CAM_AR130::setBinnedVGAMode()
{
    bool ret = false;
    u_int8_t binned_vga_status = 0;
    ret = enableBinnedVGAMode(&binned_vga_status);
    if(ret == false)
    {
        printf("Unable to switch to binned VGA Mode");
        emit deviceStatus("Failure","Unable to switch to binned VGA Mode");
        return void();
    }
    else
    {
        switch(binned_vga_status)
        {
        case 1 :
            emit deviceStatus("Success","Binned VGA mode set successfully");
            break;
        case 2 :
            emit deviceStatus("Failure","The current resolution is not 640x480, please switch to 640x480 before using the Cropping and Binning modes");
            break;
        case 3 :
            emit deviceStatus("Failure","Device is already in Binned VGA mode");
            break;
        case 4 :
            emit deviceStatus("Failure","Failed to set Binned VGA mode");
            break;
        default :
            printf("Unknown %d \n",binned_vga_status);
            emit deviceStatus("Failure","Unknown error");

        }
    }
}

void See3CAM_AR130::getFlashLevel(QString cameraName) {
    if(cameraName.isEmpty())
    {
         emit updateFlashCheckBox("0");
         return void();
    }

    quint8 flash_level = 0;
    see3cam_ctrl.getFlashState(&flash_level,cameraName);
    QString tmpStr = QString::number(flash_level);
    emit updateFlashCheckBox(tmpStr);
}
