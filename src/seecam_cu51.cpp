#include "seecam_cu51.h"


bool See3CAM_CU51::enableMasterMode()
{
    bool ret = modeControl.enableMasterMode();
    return ret;
}

bool  See3CAM_CU51::enableTriggerMode()
{
    bool ret = modeControl.enableTriggerMode();
    return ret;
}

void See3CAM_CU51::getTorchLevel(QString cameraName) {
    if(cameraName.isEmpty())
    {
         emit updateFlashCheckBox("0");
         return void();
    }

    quint8 flash_level = 0;
    see3cam_ctrl.getTorchState(&flash_level,cameraName);
    QString tmpStr = QString::number(flash_level);
    emit updateFlashCheckBox(tmpStr);
}

void See3CAM_CU51::getExposure()
{
    qDebug()<<"in getExposure";
    if(uvccamera::hid_fd < 0)
       {
           return void();
       }
           bool timeout = true;
           int ret =0;
           unsigned int start, end = 0;

           uint exposureValue;

        //Initialize the buffer
        memset(g_out_packet_buf, 0x00, sizeof(g_out_packet_buf));

        //Set the Report Number
        g_out_packet_buf[1] = CAMERA_CONTROL_51; 	/* Report Number */
        g_out_packet_buf[2] = GET_EXPOSURE_VALUE; 	/* Report Number */

        /* Send a Report to the Device */
        ret = write(uvccamera::hid_fd, g_out_packet_buf, BUFFER_LENGTH);
        if (ret < 0) {
                    perror("write");

                    return void();
                } else {
                    printf("%s(): write() wrote %d bytes\n", __func__, ret);
                }

                /* Read the Status code from the device */
                start = uvc.getTickCount();
        while(timeout)
        {
            /* Get a report from the device */
            ret = read(uvccamera::hid_fd, g_in_packet_buf, BUFFER_LENGTH);
            if (ret < 0) {
                //perror("read");
            } else {
                printf("%s(): read %d bytes:\n", __func__,ret);
                if(g_in_packet_buf[0] == CAMERA_CONTROL_51 &&
                    g_in_packet_buf[1] == GET_EXPOSURE_VALUE ) {
                        if(g_in_packet_buf[4] == EXP_SUCCESS) {
                            exposureValue = (g_in_packet_buf[3]<<8)+g_in_packet_buf[2];
                            qDebug()<<"exposureValue:"<<exposureValue;
                            emit sendExposure(exposureValue);

                            timeout = false;
                        } else if(g_in_packet_buf[4] == EXP_FAIL) {
                            return void();
                        }
                }
            }
            end = uvc.getTickCount();
                       if(end - start > TIMEOUT)
                       {
                            qDebug()<<"timeout getExposure";
                           printf("%s(): Timeout occurred\n", __func__);
                           timeout = false;
                           return void();
                       }
                   }

}

void See3CAM_CU51::setExposure(const uint &exposureValue)
{
    if(uvccamera::hid_fd < 0)
       {
           return void();
       }
    qDebug()<<"exposure passed:"<<exposureValue;
    if(exposureValue >= 1 && exposureValue <= 30000)
    {
        qDebug()<<"exposureValue set in range";
           bool timeout = true;
           int ret =0;
           unsigned int start, end = 0;

        //Initialize the buffer
        memset(g_out_packet_buf, 0x00, sizeof(g_out_packet_buf));

        //Set the Report Number
        g_out_packet_buf[1] = CAMERA_CONTROL_51; 	/* Report Number */
        g_out_packet_buf[2] = SET_EXPOSURE_VALUE; 	/* Report Number */
        g_out_packet_buf[3] = exposureValue & 0xFF;	/* LSB of Exposure value */
        g_out_packet_buf[4] = exposureValue>>8; 	/* MSB of exposure value */

        /* Send a Report to the Device */
        ret = write(uvccamera::hid_fd, g_out_packet_buf, BUFFER_LENGTH);
        if (ret < 0) {
                  perror("write");
                  return void();
              } else {
                  printf("%s(): write() wrote %d bytes\n", __func__, ret);
              }

              /* Read the Status code from the device */
              start = uvc.getTickCount();


        while(timeout)
        {
            /* Get a report from the device */
            ret = read(uvccamera::hid_fd, g_in_packet_buf, BUFFER_LENGTH);
            if (ret < 0) {
                //perror("read");
            } else {
                printf("%s(): read %d bytes:\n", __func__,ret);
                if(g_in_packet_buf[0] == CAMERA_CONTROL_51 &&
                    g_in_packet_buf[1] == SET_EXPOSURE_VALUE &&
                    g_in_packet_buf[2] == g_out_packet_buf[3] &&
                    g_in_packet_buf[3] == g_out_packet_buf[4] ) {
                        if(g_in_packet_buf[4] == EXP_SUCCESS) {
                            timeout = false;
                        } else if(g_in_packet_buf[4] == EXP_FAIL) {
                            return void();
                        }
                }
            }
            end = uvc.getTickCount();
                        if(end - start > TIMEOUT)
                        {
                            printf("%s(): Timeout occurred\n", __func__);
                            timeout = false;
                            return void();
                        }
        }
    }
    else
    {
        qDebug()<<"exposureValue not set in range";
    }
}
