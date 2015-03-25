/*
 * file description here
 */

/*****************/
/* Include Files */
/*****************/
#include "common.h"
#include "app_data_process.h"

#include <stdio.h>

/********************/
/* Defines & Macros */
/********************/


/*************/
/* Variables */
/*************/
MONITOR_DATA  monitor_data[MONITOR_NUM];
PARAM_SETTING param_setting[PARAM_SETTING_NUM];
PARAM_SETTING alarm_setting[ALARM_SETTING_NUM];
int rt_volume = 0;
int rt_pressure = 0;
int rt_airflow = 0;

/*************/
/* Functions */
/*************/
int dp_decode_package(char *package, DP_PACK_DATA *package_data)
{
    char csum = 0;
    char tpack[4];
    
    /* Check input parameter */
    if((package == NULL) || (package_data == NULL)) {
        DEBUG_PRINTF("<%s>Input parameter error.\n", __FUNCTION__);
        return (-2);
    }

    /* Cache package */
    tpack[0] = *package;
    tpack[1] = *(package+1);
    tpack[2] = *(package+2);
    tpack[3] = *(package+3);

    /* Checksum package */
    csum = tpack[0] + tpack[1] + tpack[2];
    csum = csum & 0x7F;

    if(csum != (tpack[3] & 0x7F)) {
        DEBUG_PRINTF("[INFO] Package checksum error.\n");
        return (-4);
    }
    
    /* Get channel number */
    package_data->channel = (tpack[0] & 0x7C) >> 2;

    /* Get sub channel number and value */
    switch(package_data->channel) {
        case 0x0:
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
            package_data->sub_chn = ((tpack[0] & 0x03) << 6) + ((tpack[1] & 0x7E) >> 1);
            package_data->value = ((tpack[1] & 0x01) << 7) + (tpack[2] & 0x7F);
            break;

        case 0x8:
        case 0x9:
        case 0xA:
        case 0xB:
            package_data->sub_chn = ((tpack[0] & 0x03) << 6) + ((tpack[1] & 0x60) >> 5);
            package_data->value = ((unsigned int )(tpack[1] & 0x1F) << 7) \
                                  + ((unsigned int)tpack[2] & 0x7F);
            break;

        default:
            package_data->sub_chn = 0x0;
            package_data->value = (((unsigned int)tpack[0] & 0x03) << 14) \
                                  + (((unsigned int)tpack[1] & 0x7F) << 7) \
                                  + ((unsigned int)tpack[2] & 0x7F);
            break;
    }

    return 0;
}


int dp_sort_package(DP_PACK_DATA *package_data)
{
    /* Check input parameter */
    if(package_data == NULL) {
        DEBUG_PRINTF("<%s>Input parameter error.\n", __FUNCTION__);
        return (-2);
    }

    /* */
    switch(package_data->channel) {
        //channel 8, transmit real time monitor data
        case 0x8:
            switch(package_data->sub_chn) {
                case 0x0:
                    rt_airflow = package_data->value;
                    break;

                case 0x3:
                    rt_pressure = package_data->value;
                    break;

                case 0x7:
                    monitor_data[MONITOR_TVV_IPAP].value = package_data->value;
                    monitor_data[MONITOR_TVV_IPAP].update = 1;
                    break;

                case 0x8:
                    monitor_data[MONITOR_VT].value = package_data->value;
                    monitor_data[MONITOR_VT].update = 1;
                    break;

                case 0x9:
                    monitor_data[MONITOR_MV].value = package_data->value;
                    monitor_data[MONITOR_MV].update = 1;
                    break;

                case 0xA:
                    monitor_data[MONITOR_LEAK].value = package_data->value;
                    monitor_data[MONITOR_LEAK].update = 1;
                    break;

                case 0xB:
                    monitor_data[MONITOR_RR].value = package_data->value;
                    monitor_data[MONITOR_RR].update = 1;
                    break;

                case 0xC:
                    monitor_data[MONITOR_IT].value = package_data->value;
                    monitor_data[MONITOR_IT].update = 1;
                    break;

                case 0xD:
                    monitor_data[MONITOR_IE].value = package_data->value;
                    monitor_data[MONITOR_IE].update = 1;
                    break;

                case 0xE:
                    monitor_data[MONITOR_TS].value = package_data->value >> 8;
                    monitor_data[MONITOR_TS].update = 1;
                    monitor_data[MONITOR_TR].value = package_data->value & 0xFF;
                    monitor_data[MONITOR_TR].update = 1;
                    break;

                case 0xF:
                    rt_volume = package_data->value;
                    break;
            }
            break;

        case 0x1E:
            sys_rtc.value = (sys_rtc.value & 0x0000FFFF) + ((unsigned long)package_data->value << 16);
            sys_rtc.update = (sys_rtc.update & 0x01) | 0x10;
            break;

        case 0x1F:
            sys_rtc.value = (sys_rtc.value & 0xFFFF0000) + (unsigned long)package_data->value;
            sys_rtc.update = (sys_rtc.update & 0x10) | 0x01;
            break;

        case 0x1A:
            cb_alarm_info = (cb_alarm_info & 0xFFFF0000) + (unsigned long)package_data->value;
            break;

        case 0x1B:
            cb_alarm_info = (cb_alarm_info & 0x0000FFFF) + ((unsigned long)package_data->value << 16);
            break;

        case 0x1C:
            cb_sys_status = (cb_sys_status & 0xFFFF0000) + (unsigned long)package_data->value;
            break;

        case 0x1D:
            cb_sys_status = (cb_sys_status & 0x0000FFFF) + ((unsigned long)package_data->value << 16);
            break;

        case 0x5:
            switch(package_data->sub_chn) {
                case 0x5A:
                    if(package_data->value == 0xA5) {
                        cb_handshake = 1;
                    }
                    break;

                case 0x69:
                    if(package_data->value == 0x96) {
                        cb_trans_rt_data = 1;
                    }
                    else if(package_data->value == 0x69) {
                        cb_trans_rt_data = 0;
                    }
                    break;
            }
        //channel 0
        case 0x0:
            switch(package_data->sub_chn) {
                //patient trigger to run
                case 0x0:
                    if(package_data->value == 0xA5) {
                        patient_trigger_run = 1;
                    }
                    break;
            }
            break;
    }

}


int dp_code_package(DP_PACK_DATA *package_data, char *package)
{
    char tpack[4];
    //char ctemp = 0;
    int i = 0, j = 0, t = 0;
    char csum = 0;

    /* Check input parameter */
    if((package_data == NULL) || (package == NULL)) {
        DEBUG_PRINTF("<%s>Input parameter error.\n", __FUNCTION__);
        return (-2);
    }

    /* Makeup load data of the package */
    if((package_data->channel >= 0) && (package_data->channel < 8)) {
        t = ((int)package_data->sub_chn << 8) + (package_data->value & 0xFF);
    }
    else if((package_data->channel >=8) && (package_data->channel < 12)) {
        t = ((int)(package_data->sub_chn & 0xF) << 12) + (package_data->value & 0x3FF);
    }
    else {
        t = package_data->value;
    }

    /* Makeup package */    
    tpack[0] = 0x80 + ((package_data->channel & 0x1F) << 2) + (char)((t & 0xC000) >> 14);
    tpack[1] = 0x00 + (char)((t & 0x3F80) >> 7);
    tpack[2] = 0x00 + (char)(t & 0x7F);
    //make package checksum
    for(i=0; i<3; i++) {
        for(j=7; j>=0; j--) {
            csum += (tpack[i] >> j) & 0x01;
        }
    }
    tpack[3] = 0x00 + (csum & 0x7F);

    for(i=0; i<4; i++) {
        *(package+i) = tpack[i];
    }

    return 0;
}


