#include <stddef.h>                     // Defines NULL
#include <string.h>
#include <xc.h>

#include "definitions.h"                // SYS function prototypes
#include "PMS7003.h"
#include "CustomTime.h"
#include "debug.h"

bool PMS_Init(void){
    
    bool init_state = false;
    PMS_Activation();
    init_state = PMS_Wake_Up();
    if(init_state)
        init_state = PMS_Set_Passive_Mode();
 
    if(!init_state)
        DEBUG_PRINT("PMS INIT ERR");
    
    return init_state;
}

void PMS_Activation(void){
    if(POWER_5V_EN_Get() == 0){
        POWER_5V_EN_Toggle();
    }
    if(PMS_EN_Get() == 0){
        PMS_EN_Toggle();
    }
    
    DelayMs(100);
}

void PMS_Deactivation(void){
    if(POWER_5V_EN_Get() == 1){
        POWER_5V_EN_Toggle();
    }
    if(PMS_EN_Get() == 1){
        PMS_EN_Toggle();
    }
    
    DelayMs(10);
}

bool PMS_Set_Passive_Mode(void){
    bool ret = false;
    uint8_t passive_mode_set[PMS_CMD_LEN]={0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70};
    
    ret = UART3_Write(passive_mode_set, PMS_CMD_LEN);
    PMS_Wait_4_Tx();                       //Wait for TX 
    
    return ret;
}

bool PMS_Set_Sleep_Mode(void){
    bool ret = false;
    uint8_t pms_sleep[PMS_CMD_LEN]={0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73};
    
    ret = UART3_Write(pms_sleep, PMS_CMD_LEN);
    PMS_Wait_4_Tx();                       //Wait for TX
    
    return ret;
}

bool PMS_Wake_Up(void){
    bool ret = false;
    uint8_t pms_wakeup[PMS_CMD_LEN]={0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74};
    
    ret = UART3_Write(pms_wakeup, PMS_CMD_LEN);
    PMS_Wait_4_Tx();                       //Wait for TX
    
    return ret;
}

bool PMS_Passive_Read(uint16_t *pm1, uint16_t *pm25, uint16_t *pm10){
    bool ret = false;
    uint8_t passive_read[PMS_CMD_LEN]={0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71};
    uint8_t pms_lecture[32];
    
    memset(pms_lecture, 0, PMS_RESPONSE_SIZE);
    ret = UART3_Write(passive_read, PMS_CMD_LEN);
    PMS_Wait_4_Tx();
    if(!ret)
        return ret;
    UART3_Read(pms_lecture, PMS_RESPONSE_SIZE);
    PMS_Wait_4_Rx();
    if(!ret)
        return ret;
    
    if( (pms_lecture[0] == 0x42) && (pms_lecture[0] == 0x42)){
        *pm1 = (pms_lecture[4]<<8) + pms_lecture[5];
        *pm25 = (pms_lecture[6]<<8) + pms_lecture[7];
        *pm10 = (pms_lecture[8]<<8) + pms_lecture[9];
    } else {
        *pm1 = 0;
        *pm25 = 0;
        *pm10 = 0;
    }
    
    return ret;
}

bool PMS_Wait_4_Rx(void){
    
    bool ret = true;
    uint32_t tWait = ( GetSystemClock() / 2000 ) * UART_ERR_TIMEOUT_MS; //Delay for timeout
    uint32_t tStart = ReadCoreTimer(); 
    
    while( ((ReadCoreTimer() - tStart) < tWait) && (!PMS_Data_Received) );
    if(!PMS_Data_Received)
        ret = false;
    else
        PMS_Data_Received = false;
    
    return ret;
}

bool PMS_Wait_4_Tx(void){
    
    bool ret = true;
    uint32_t tWait = ( GetSystemClock() / 2000 ) * UART_ERR_TIMEOUT_MS; //Delay for timeout
    uint32_t tStart = ReadCoreTimer(); 
    
    while( ((ReadCoreTimer() - tStart) < tWait) && (!PMS_Data_Sent) );
    if(!PMS_Data_Sent)
        ret = false;
    else
        PMS_Data_Sent = false;
    
    return ret; 
}