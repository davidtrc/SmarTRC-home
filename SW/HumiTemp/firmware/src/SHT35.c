#include <math.h>
#include <stddef.h>                     // Defines NULL
#include <string.h>
#include <xc.h>

#include "definitions.h"                // SYS function prototypes
#include "SHT35.h"
#include "CustomTime.h"
#include "crc8.h"
#include "debug.h"

bool SHT_Init(void){
    bool ret = SHT_Set_Single_Shot_Mode(true, LOW_REPETEABILITY);
    if(!ret)
        DEBUG_PRINT("SHT INIT ERR");
    
    return ret;
}

bool SHT_Get_Temp_and_Humidity(double *tempd, double *humid){
    
    bool ret = false;
    uint8_t sensor_measure[2] = {0x2C, 0x06};
    uint8_t sensor_lecture[6];
    uint16_t temp, humi;
    
    memset(sensor_lecture, 0, sizeof(sensor_lecture));
    ret = I2C3_Write((uint8_t)SENSOR_ADDRESS, sensor_measure, sizeof(sensor_measure));
    while(I2C3_IsBusy());
    if(!ret){
        *tempd = 0;
        *humid = 0;
        return ret;
    }
    DelayMs(16);            //Maximum time to perform a measure
    ret = I2C3_Read((uint8_t)SENSOR_ADDRESS, sensor_lecture, sizeof(sensor_lecture));
    while(I2C3_IsBusy());
    if(!ret){
        *tempd = 0;
        *humid = 0;
        return ret;
    }

    temp = (sensor_lecture[0]<<8) + sensor_lecture[1];
    humi = (sensor_lecture[3]<<8) + sensor_lecture[4];

    *tempd = ((175*temp)/(pow(2,16)-1)) - 45;
    *humid = (100*humi)/(pow(2,16)-1);
    
    return ret;
}

bool SHT_SoftReset(void){
    
    bool ret = false;
    
    uint8_t reset_cmd[2] = {0x30, 0xA2};
    
    ret = I2C3_Write((uint8_t)SENSOR_ADDRESS, reset_cmd, sizeof(reset_cmd));
    while(I2C3_IsBusy());
    
    return ret;
}

bool SHT_Set_Single_Shot_Mode(bool clock_stretching, uint8_t repeteability){
    
    bool ret = false;
    uint8_t SS_cmd[2] = {0x00, 0x00};
    
    if(clock_stretching){
        SS_cmd[0] = 0x2C;
        
        if(repeteability == HIGH_REPETEABILITY)             //High repeatibility
            SS_cmd[1] = 0x06;
        else if(repeteability == MEDIUM_REPETEABILITY)      //Medium repeatibility
            SS_cmd[1] = 0x0D;
        else if(repeteability == LOW_REPETEABILITY)         //Low repeatibility
            SS_cmd[1] = 0x10;
    } else {
       SS_cmd[0] = 0x24; 
       
        if(repeteability == HIGH_REPETEABILITY)              //High repeatibility
            SS_cmd[1] = 0x00;
        else if(repeteability == MEDIUM_REPETEABILITY)       //Medium repeatibility
            SS_cmd[1] = 0x0B;
        else if(repeteability == LOW_REPETEABILITY)          //Low repeatibility
            SS_cmd[1] = 0x16;
    }
    
    ret = I2C3_Write((uint8_t)SENSOR_ADDRESS, SS_cmd, sizeof(SS_cmd));
    while(I2C3_IsBusy());
    
    return ret;
}

bool SHT_Set_Periodic_Shot_Mode(uint8_t measures_per_second, uint8_t repeteability){
    
    bool ret = false;
    uint8_t PS_cmd[2] = {0x00, 0x00};
    
    if(measures_per_second == MPS_05){  //0.5
        PS_cmd[0] = 0x20;
        
        if(repeteability == HIGH_REPETEABILITY)             //High repeatibility
            PS_cmd[1] = 0x32;
        else if(repeteability == MEDIUM_REPETEABILITY)      //Medium repeatibility
            PS_cmd[1] = 0x24;
        else if(repeteability == LOW_REPETEABILITY)         //Low repeatibility
            PS_cmd[1] = 0x2F;
    } else if(measures_per_second == MPS_1){//1
       PS_cmd[0] = 0x21; 
       
        if(repeteability == HIGH_REPETEABILITY)             //High repeatibility
            PS_cmd[1] = 0x30;
        else if(repeteability == MEDIUM_REPETEABILITY)      //Medium repeatibility
            PS_cmd[1] = 0x26;
        else if(repeteability == LOW_REPETEABILITY)         //Low repeatibility
            PS_cmd[1] = 0x2D;
    }  else if(measures_per_second == MPS_2){ //2
       PS_cmd[0] = 0x22; 
       
        if(repeteability == HIGH_REPETEABILITY)             //High repeatibility
            PS_cmd[1] = 0x36;
        else if(repeteability == MEDIUM_REPETEABILITY)      //Medium repeatibility
            PS_cmd[1] = 0x20;
        else if(repeteability == LOW_REPETEABILITY)         //Low repeatibility
            PS_cmd[1] = 0x2B;
    }  else if(measures_per_second == MPS_4){ //4
       PS_cmd[0] = 0x23; 
       
        if(repeteability == HIGH_REPETEABILITY)             //High repeatibility
            PS_cmd[1] = 0x34;
        else if(repeteability == MEDIUM_REPETEABILITY)      //Medium repeatibility
            PS_cmd[1] = 0x22;
        else if(repeteability == LOW_REPETEABILITY)         //Low repeatibility
            PS_cmd[1] = 0x29;
    }  else if(measures_per_second == MPS_10){ //10
       PS_cmd[0] = 0x27; 
       
        if(repeteability == HIGH_REPETEABILITY)             //High repeatibility
            PS_cmd[1] = 0x37;
        else if(repeteability == MEDIUM_REPETEABILITY)      //Medium repeatibility
            PS_cmd[1] = 0x21;
        else if(repeteability == LOW_REPETEABILITY)         //Low repeatibility
            PS_cmd[1] = 0x2A;
    }
    
    ret = I2C3_Write((uint8_t)SENSOR_ADDRESS, PS_cmd, sizeof(PS_cmd));
    while(I2C3_IsBusy());
    
    return ret;
}

bool SHT_Stop_Periodic_Shot_Mode(void){
    
    bool ret = false;
    uint8_t PS_cmd[2] = {0x30, 0x93};
    
    ret = I2C3_Write((uint8_t)SENSOR_ADDRESS, PS_cmd, sizeof(PS_cmd));
    while(I2C3_IsBusy());
    
    return ret; 
}

bool SHT_Get_Status_Register(uint16_t *reg_val){
   
    uint8_t SR_cmd[2] = {0xF3, 0x2D};
    uint8_t SR_lecture[3] = {0x00, 0x00, 0x00};
    uint16_t SR;
    bool ret = false;
    
    ret = I2C3_Write((uint8_t)SENSOR_ADDRESS, SR_cmd, sizeof(SR_cmd));
    while(I2C3_IsBusy());
    if(!ret)
        return ret;
    
    ret = I2C3_Read((uint8_t)SENSOR_ADDRESS, SR_lecture, sizeof(SR_lecture));
    while(I2C3_IsBusy());
    if(!ret)
        return ret;
    
    SR = (SR_lecture[0]<<8) + SR_lecture[1];
    *reg_val = SR;
    
    return ret;
}

bool SHT_Clear_Status_Register(void){
    
    uint8_t CSR_cmd[2] = {0x30, 0x41};
    bool ret = false;
    
    ret = I2C3_Write((uint8_t)SENSOR_ADDRESS, CSR_cmd, sizeof(CSR_cmd));
    while(I2C3_IsBusy());
    
    return ret;
}

bool SHT_Enable_Alert_Mode(uint16_t temp_HI_SET, uint16_t temp_HI_CLEAR, uint16_t temp_LO_SET, uint16_t temp_LO_CLEAR, 
                           uint16_t humi_HI_SET, uint16_t humi_HI_CLEAR, uint16_t humi_LO_SET, uint16_t humi_LO_CLEAR){         //Only valid in periodic mode
    
    uint8_t ALERT_cmd[5] = {0x61, 0x00, 0x00, 0x00, 0x00};
    uint16_t cmd_value = 0;
    bool ret = false;
    
    ALERT_cmd[1] = 0x1D;                                            //High alert limit set (hysteresis cycle upper value)
    cmd_value = (humi_HI_SET & 0xFE00) | (temp_HI_SET>>7);          //RH and temp are sent in the same command
    ALERT_cmd[2] = (uint8_t)(cmd_value>>8);
    ALERT_cmd[3] = (uint8_t)(cmd_value);
    ALERT_cmd[4] = Crc8Block(&ALERT_cmd[2], 2, POLYNOMIAL2, 0xFF);
    ret = I2C3_Write((uint8_t)SENSOR_ADDRESS, ALERT_cmd, sizeof(ALERT_cmd));
    while(I2C3_IsBusy());
    if(!ret)
        return ret;
    
    ALERT_cmd[1] = 0x16;                                            //High alert limit clear (hysteresis cycle lower value)
    cmd_value = (humi_HI_CLEAR & 0xFE00) | (temp_HI_CLEAR>>7);      //RH and temp are sent in the same command
    ALERT_cmd[2] = (uint8_t)(cmd_value>>8);
    ALERT_cmd[3] = (uint8_t)(cmd_value);
    ALERT_cmd[4] = Crc8Block(&ALERT_cmd[2], 2, POLYNOMIAL2, 0xFF);
    ret = I2C3_Write((uint8_t)SENSOR_ADDRESS, ALERT_cmd, sizeof(ALERT_cmd));
    while(I2C3_IsBusy());
    if(!ret)
        return ret;
    
    ALERT_cmd[1] = 0x0B;                                            //Low alert limit set (hysteresis cycle upper value)
    cmd_value = (humi_LO_SET & 0xFE00) | (temp_LO_SET>>7);          //RH and temp are sent in the same command
    ALERT_cmd[2] = (uint8_t)(cmd_value>>8);
    ALERT_cmd[3] = (uint8_t)(cmd_value);
    ALERT_cmd[4] = Crc8Block(&ALERT_cmd[2], 2, POLYNOMIAL2, 0xFF);
    ret = I2C3_Write((uint8_t)SENSOR_ADDRESS, ALERT_cmd, sizeof(ALERT_cmd));
    while(I2C3_IsBusy());
    if(!ret)
        return ret;
    
    ALERT_cmd[1] = 0x00;                                            //Low alert limit clear (hysteresis cycle lower value)
    cmd_value = (humi_LO_CLEAR & 0xFE00) | (temp_LO_CLEAR>>7);      //RH and temp are sent in the same command
    ALERT_cmd[2] = (uint8_t)(cmd_value>>8);
    ALERT_cmd[3] = (uint8_t)(cmd_value);
    ALERT_cmd[4] = Crc8Block(&ALERT_cmd[2], 2, POLYNOMIAL2, 0xFF);
    ret = I2C3_Write((uint8_t)SENSOR_ADDRESS, ALERT_cmd, sizeof(ALERT_cmd));
    while(I2C3_IsBusy());

    return ret;
}

bool SHT_Get_Alert_Thresholds(uint16_t *temp_HI_SET, uint16_t *temp_HI_CLEAR, uint16_t *temp_LO_SET, uint16_t *temp_LO_CLEAR, 
                              uint16_t *humi_HI_SET, uint16_t *humi_HI_CLEAR, uint16_t *humi_LO_SET, uint16_t *humi_LO_CLEAR){  //Only valid in periodic mode
    
    uint8_t ALERT_cmd[2] = {0xE1, 0x00};
    uint8_t ALERT_cmd_response[3] = {0x00, 0x00, 0x00};
    uint16_t resp_value = 0;
    bool ret = false;
    
    ALERT_cmd[1] = 0x1F;                                            //High alert limit set (hysteresis cycle upper value)
    ret = I2C3_WriteRead((uint8_t)SENSOR_ADDRESS, ALERT_cmd, sizeof(ALERT_cmd), ALERT_cmd_response, sizeof(ALERT_cmd_response));
    while(I2C3_IsBusy());
    if(!ret)
        return ret;
    resp_value = ( (uint16_t)(ALERT_cmd_response[0]<<8) ) | (ALERT_cmd_response[1]);          //RH and temp are sent in the same command
    *humi_HI_SET = resp_value & 0xFE00;
    *temp_HI_SET = (resp_value & 0x01FF)<<7;

    ALERT_cmd[1] = 0x14;                                            //High alert limit clear (hysteresis cycle upper value)
    ret = I2C3_WriteRead((uint8_t)SENSOR_ADDRESS, ALERT_cmd, sizeof(ALERT_cmd), ALERT_cmd_response, sizeof(ALERT_cmd_response));
    while(I2C3_IsBusy());
    if(!ret)
        return ret;
    resp_value = ( (uint16_t)(ALERT_cmd_response[0]<<8) ) | (ALERT_cmd_response[1]);          //RH and temp are sent in the same command
    *humi_HI_CLEAR = resp_value & 0xFE00;
    *temp_HI_CLEAR = (resp_value & 0x01FF)<<7;   

    ALERT_cmd[1] = 0x09;                                            //High alert limit set (hysteresis cycle upper value)
    ret = I2C3_WriteRead((uint8_t)SENSOR_ADDRESS, ALERT_cmd, sizeof(ALERT_cmd), ALERT_cmd_response, sizeof(ALERT_cmd_response));
    while(I2C3_IsBusy());
    if(!ret)
        return ret;
    resp_value = ( (uint16_t)(ALERT_cmd_response[0]<<8) ) | (ALERT_cmd_response[1]);          //RH and temp are sent in the same command
    *humi_LO_SET = resp_value & 0xFE00;
    *temp_LO_SET = (resp_value & 0x01FF)<<7;
    
    ALERT_cmd[1] = 0x02;                                            //High alert limit set (hysteresis cycle upper value)
    ret = I2C3_WriteRead((uint8_t)SENSOR_ADDRESS, ALERT_cmd, sizeof(ALERT_cmd), ALERT_cmd_response, sizeof(ALERT_cmd_response));
    while(I2C3_IsBusy());
    if(!ret)
        return ret;
    resp_value = ( (uint16_t)(ALERT_cmd_response[0]<<8) ) | (ALERT_cmd_response[1]);          //RH and temp are sent in the same command
    *humi_LO_CLEAR = resp_value & 0xFE00;
    *temp_LO_CLEAR = (resp_value & 0x01FF)<<7;
    
    return ret;
}

bool SHT_Disable_Alert_Mode(void){

    bool ret = false;
    
    ret = SHT_Enable_Alert_Mode(12, 11, 15, 16, 50, 45, 60, 55);        //Only can be done by setting low threshold upper than high threshold
    
    return ret;
}