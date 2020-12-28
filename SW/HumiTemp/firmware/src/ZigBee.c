#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdio.h>
#include <string.h>

#include "definitions.h"                // SYS function prototypes
#include "CustomTime.h"
#include "ZigBee.h"
#include "crc8.h"
#include "debug.h"

uint8_t ZigBee_Get_Short_Register_Value(uint8_t register_in){
    
    uint8_t register_rd[ZIGBEE_SHORT_RD_MSG_BYTES_SIZE];
    
    memset(register_rd, 0, sizeof(register_rd));
    register_rd[0] = (register_in<<1)+ZIGBEE_RD_BIT;    

    if(!(SPI1_WriteRead(register_rd, ZIGBEE_WR_SHORT_RQST_BYTES_SIZE, &register_rd[ZIGBEE_WR_SHORT_RQST_BYTES_SIZE], ZIGBEE_RESPONSE_BYTES_SIZE)))
        DEBUG_PRINT("WR ZIGBEE ERR NO IDLE IN GET_REG");
    while(SPI1_IsBusy());
    
    return register_rd[1];
}

bool ZigBee_Set_Short_Register_Value(uint8_t register_in, uint8_t reg_val, bool check_correct_write){
    
    bool ret = false;

    uint8_t cmd_val[ZIGBEE_SHORT_WR_MSG_BYTES_SIZE]={((register_in<<1)+ZIGBEE_WR_BIT), reg_val};
    
    ret = SPI1_Write(cmd_val, ZIGBEE_SHORT_WR_MSG_BYTES_SIZE);
    while(SPI1_IsBusy());
    if(!ret){
        DEBUG_PRINT("WR ZIGBEE ERR NO IDLE IN SET_REG");
        return ret;
    }     
    
    if(check_correct_write){
        uint8_t rd = ZigBee_Get_Short_Register_Value(register_in);
        while(SPI1_IsBusy());
        
        if( reg_val != rd){
            ret = true;
            DEBUG_PRINT("WR ZIGBEE ERR REG=%X RD=%X WR=%X\r\n", register_in, rd, reg_val);
        }
    }

    return ret;
}

/* *****************************************************************************
 End of File
 */
