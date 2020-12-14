#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdio.h>
#include <string.h>

#include "definitions.h"                // SYS function prototypes
#include "LC709203F.h"
#include "CustomTime.h"
#include "crc8.h"
#include "debug.h"
    #define DEBUG

uint8_t LC709203F_Init(void){
    
    uint8_t ret = 0;
    
    if(LC709203F_Set_Operational_Mode(OPERATIONAL_MODE_VAL, true))  //Enables the device
        ret = ret | SET_OM_ERR;   
    if(LC709203F_Set_APA(APA_VAL, true))                                //Sets APA value and checks that it writes correctly
        ret = ret | SET_APA_ERR;  
    if(LC709203F_Set_Battery_Profile(CHANGE_OF_PARAMETER_VAL, true))         //Sets Change of paramter and checks that it writes correctly
        ret = ret | SET_BP_ERR;  
    LC709203F_Set_Initial_RSOC(INITIAL_RSOC_VAL);                         //Configures the initial RSOC 
    if(LC709203F_Set_I2C_Mode(GAUGE_STATUS_BIT_VAL, true))             //Enables I2C mode for the temperature
        ret = ret | SET_IM_ERR;  
    if(LC709203F_Set_Temperature(DEFAULT_TEMP, true))                  //Sets the temperature
        ret = ret | SET_TEMP_ERR;  
    
    return ret;
}

uint16_t LC709203F_Get_Register_Value(uint8_t register_in){
    uint8_t register_rd[1] = {register_in};
    uint8_t lecture[RD_MSG_BYTES_SIZE];
    uint16_t ret = 0;
    
    memset(lecture, 0, sizeof(lecture));

    if(I2C1_WriteRead((uint16_t)GAUGE_ADDRESS, register_rd, 1, lecture, RD_MSG_BYTES_SIZE))
        DEBUG_PRINT("WR LC709203F ERR NO IDLE IN GET_REG");
    DelayMs(50);
    ret = (lecture[1]<<8 | lecture[0]);
    
    return ret;
}

bool LC709203F_Set_Register_Value(uint8_t register_in, uint8_t reg_val_LOW, uint8_t reg_val_HIGH, bool check_correct_write){
    bool ret = false;

    uint8_t cmd_val[WR_MSG_BYTES_SIZE+1]={GAUGE_ADDRESS<<1, register_in, reg_val_LOW, reg_val_HIGH, 0};
    
    cmd_val[WR_MSG_BYTES_SIZE] = Crc8Block(cmd_val, WR_MSG_BYTES_SIZE);
    
    ret = I2C1_Write((uint16_t)GAUGE_ADDRESS, &cmd_val[1], WR_MSG_BYTES_SIZE);
    DelayMs(50);
    if(ret){
        DEBUG_PRINT("WR LC709203F ERR NO IDLE IN SET_REG");
        return ret;
    }     
    
    if(check_correct_write){
        uint16_t rd = LC709203F_Get_Register_Value(register_in);
        DelayMs(50);
        uint16_t wr_val = cmd_val[3]<<8 | cmd_val[2];
        
        if( wr_val != rd){
            ret = true;
            DEBUG_PRINT("WR LC709203F ERR REG=%X RD=%X WR=%X\r\n", register_in, wr_val, rd);
        }
    }

    return ret;
}

uint16_t LC709203F_Get_Operational_Mode(void){
    uint16_t OM_register = LC709203F_Get_Register_Value(OPERATIONAL_MODE_REG);
    
    return OM_register;
}

bool LC709203F_Set_Operational_Mode(uint16_t OM_value, bool check_correct_write){
    bool ret = false;
    
    ret = LC709203F_Set_Register_Value(OPERATIONAL_MODE_REG, (uint8_t)OM_value, (uint8_t)OM_value>>8, check_correct_write);
    
    return ret;
}

uint16_t LC709203F_Get_APA(void){
    uint16_t APA_register = LC709203F_Get_Register_Value(APA_REG);
    
    return APA_register;
}

bool LC709203F_Set_APA(uint16_t OM_value, bool check_correct_write){
    bool ret = false;
    
    ret = LC709203F_Set_Register_Value(APA_REG, (uint8_t)OM_value, (uint8_t)OM_value>>8, check_correct_write);
    
    return ret;
}

uint16_t LC709203F_Get_Battery_Profile(void){
    uint16_t BP_register = LC709203F_Get_Register_Value(CHANGE_OF_PARAMETER_REG);
    
    return BP_register;
}

bool LC709203F_Set_Battery_Profile(uint16_t OM_value, bool check_correct_write){
    bool ret = false;
    
    ret = LC709203F_Set_Register_Value(CHANGE_OF_PARAMETER_REG, (uint8_t)OM_value, (uint8_t)OM_value>>8, check_correct_write);
    
    return ret;
}

bool LC709203F_Set_Initial_RSOC(uint16_t OM_value){
    bool ret = false;
    
    ret = LC709203F_Set_Register_Value(CHANGE_OF_PARAMETER_REG, (uint8_t)OM_value, (uint8_t)OM_value>>8, false);
    
    return ret;
}

uint16_t LC709203F_Get_I2C_Mode(void){
    uint16_t PM_register = LC709203F_Get_Register_Value(GAUGE_STATUS_BIT_REG);
    
    return PM_register;
}

bool LC709203F_Set_I2C_Mode(uint16_t OM_value, bool check_correct_write){
    bool ret = false;
    
    ret = LC709203F_Set_Register_Value(GAUGE_STATUS_BIT_REG, (uint8_t)OM_value, (uint8_t)OM_value>>8, check_correct_write);
    
    return ret;
}

uint16_t LC709203F_Get_Temperature(void){
    uint16_t PM_register = LC709203F_Get_Register_Value(CELL_TEMPERATURE_REG);
    
    return PM_register;
}

bool LC709203F_Set_Temperature(uint16_t OM_value, bool check_correct_write){
    bool ret = false;
    
    ret = LC709203F_Set_Register_Value(CELL_TEMPERATURE_REG, (uint8_t)OM_value, (uint8_t)OM_value>>8, check_correct_write);
    
    return ret;
}

uint16_t LC709203F_Get_Cell_Voltage(void){
    uint16_t OM_register = LC709203F_Get_Register_Value(CELL_VOLTAGE_REG);
    
    return OM_register;
}

uint16_t LC709203F_Get_RSOC(void){
    uint16_t OM_register = LC709203F_Get_Register_Value(RSOC_REG);
    
    return OM_register;
}