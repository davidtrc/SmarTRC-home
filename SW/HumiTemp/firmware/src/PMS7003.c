#include <stddef.h>                     // Defines NULL
#include <string.h>
#include <xc.h>

#include "definitions.h"                // SYS function prototypes
#include "PMS7003.h"
#include "CustomTime.h"

void PMS_Activation(){
    if(POWER_5V_EN_Get() == 0){
        POWER_5V_EN_Toggle();
    }
    if(PMS_EN_Get() == 0){
        PMS_EN_Toggle();
    }
    
    DelayMs(200);
}

void PMS_Deactivation(){
    if(POWER_5V_EN_Get() == 1){
        POWER_5V_EN_Toggle();
    }
    if(PMS_EN_Get() == 1){
        PMS_EN_Toggle();
    }
    
    DelayMs(200);
}

void PMS_Set_Passive_Mode(){
    uint8_t passive_mode_set[PMS_CMD_LEN]={0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70};
    
    UART3_Write(passive_mode_set, PMS_CMD_LEN);
    DelayMs(200);                       //Wait for change
}

void PMS_Set_Sleep_Mode(){
    uint8_t pms_sleep[PMS_CMD_LEN]={0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73};
    
    UART3_Write(pms_sleep, PMS_CMD_LEN);
    DelayMs(200);                       //Wait for change
}

void PMS_Wake_Up(){
    uint8_t pms_wakeup[PMS_CMD_LEN]={0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74};
    
    UART3_Write(pms_wakeup, PMS_CMD_LEN);
    DelayMs(200);                       //Wait for change
}

void PMS_Passive_Read(uint16_t *pm1, uint16_t *pm25, uint16_t *pm10){
    uint8_t passive_read[PMS_CMD_LEN]={0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71};
    uint8_t pms_lecture[32];
    
    memset(pms_lecture, 0, PMS_RESPONSE_SIZE);
    UART3_Write(passive_read, PMS_CMD_LEN);
    UART3_Read(pms_lecture, PMS_RESPONSE_SIZE);
    
    if( (pms_lecture[0] == 0x42) && (pms_lecture[0] == 0x42)){
        *pm1 = (pms_lecture[4]<<8) + pms_lecture[5];
        *pm25 = (pms_lecture[6]<<8) + pms_lecture[7];
        *pm10 = (pms_lecture[8]<<8) + pms_lecture[9];
    } else {
        *pm1 = 0;
        *pm25 = 0;
        *pm10 = 0;
    }
}