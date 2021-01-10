#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "settings.h"
#include "definitions.h"
#include "config/default/peripheral/nvm/plib_nvm.h"

settings_t global_settings;
uint32_t settings[TOTAL_SETTINGS] CACHE_ALIGN;

bool is_Settings_First_Boot(void){  
    uint32_t reg;
    
    NVM_Read(&reg, 4, SETTINGS_START_ADDRESS);
    while(NVM_IsBusy());
    
    if(reg != SETTINGS_FIRST_BOOT_PATTERN)
        return true;
    
    return false;
}

void load_NVM_Settings(){
    uint32_t reg;
    
    NVM_Read(&reg, 4, SETTINGS_WAKEUP_TIME_ADDRESS);
    while(NVM_IsBusy()); 
    global_settings.wakeup_time = (uint8_t)reg;
    
    NVM_Read(&reg, 4, SETTINGS_HEATING_TEMP_ADDRESS);
    while(NVM_IsBusy()); 
    global_settings.heating_temp = (double)reg/10.0;   
    
    NVM_Read(&reg, 4, SETTINGS_ZIGBEE_MASTERID_ADDRESS);
    while(NVM_IsBusy()); 
    global_settings.ZigBee_MasterID = (uint16_t)reg;   
}

void set_NVM_Settings(uint32_t WAKEUP_TIME, uint32_t HEATING_TEMP, uint32_t ZIGBEE_MASTERID){
    
    NVM_PageErase(SETTINGS_START_ADDRESS);
    DEBUG_PRINT("Erased at %X", SETTINGS_START_ADDRESS);
    
    memset(&settings, 0xFF, sizeof(settings));
    
    settings[0] = SETTINGS_FIRST_BOOT_PATTERN;
    settings[1] = WAKEUP_TIME;
    settings[2] = HEATING_TEMP;
    settings[3] = ZIGBEE_MASTERID;

    NVM_WordWrite(settings[0], SETTINGS_START_ADDRESS);
    while(NVM_IsBusy());
    NVM_WordWrite(settings[1], SETTINGS_WAKEUP_TIME_ADDRESS);
    while(NVM_IsBusy());
    NVM_WordWrite(settings[2], SETTINGS_HEATING_TEMP_ADDRESS);
    while(NVM_IsBusy());
    NVM_WordWrite(settings[3], SETTINGS_ZIGBEE_MASTERID_ADDRESS);
    while(NVM_IsBusy());
    
    global_settings.wakeup_time = (uint8_t)WAKEUP_TIME;
    global_settings.heating_temp = (double)HEATING_TEMP/10.0;
    global_settings.ZigBee_MasterID = (uint16_t)ZIGBEE_MASTERID;
}

void set_Default_NVM_Settings(void){ 
    set_NVM_Settings(SETTINGS_WAKEUP_TIME_DEFAULT, SETTINGS_HEATING_TEMP_DEFAULT, SETTINGS_ZIGBEE_MASTERID_DEFAULT);
}

void set_Default_Settings(void){
    global_settings.hour = 0;
    global_settings.minute = 0;
    global_settings.month = 1;
    global_settings.year = 2021;
}

uint8_t get_NVM_WakeUp_Time(void){    
    uint32_t reg = 0;
    
    NVM_Read(&reg, 4, SETTINGS_WAKEUP_TIME_ADDRESS);
    
    uint8_t ret = (uint8_t) reg;
    
    return ret;
}

void set_NVM_WakeUp_Time(uint8_t new_time){    
    uint32_t reg1 = (uint32_t)new_time;
    uint32_t reg2;
    uint32_t reg3;
    
    NVM_Read(&reg2, 4, SETTINGS_HEATING_TEMP_ADDRESS);
    NVM_Read(&reg3, 4, SETTINGS_ZIGBEE_MASTERID_ADDRESS);
    set_NVM_Settings(reg1, reg2, reg3);
}

double get_NVM_Heating_Temp(void){    
    uint32_t reg = 0;
    
    NVM_Read(&reg, 4, SETTINGS_HEATING_TEMP_ADDRESS);
    
    double ret = (double) reg/10.0;
    
    return ret;
}

void set_NVM_Heating_Temp(double new_temp){    
    uint32_t reg1;
    uint32_t reg2 = (uint32_t)(new_temp*10);
    uint32_t reg3;
    
    NVM_Read(&reg1, 4, SETTINGS_WAKEUP_TIME_ADDRESS);
    NVM_Read(&reg3, 4, SETTINGS_ZIGBEE_MASTERID_ADDRESS);
    set_NVM_Settings(reg1, reg2, reg3);
}

uint16_t get_NVM_ZigBee_MasterID(void){    
    uint32_t reg;
    
    NVM_Read(&reg, 4, SETTINGS_ZIGBEE_MASTERID_ADDRESS);

    uint16_t ret = (uint16_t) reg;
   
    return ret;
}

void set_NVM_ZigBee_MasterID(uint16_t new_ID){    
    uint32_t reg1;
    uint32_t reg2;
    uint32_t reg3 = (uint32_t)(new_ID);
    
    NVM_Read(&reg1, 4, SETTINGS_WAKEUP_TIME_ADDRESS);
    NVM_Read(&reg2, 4, SETTINGS_HEATING_TEMP_ADDRESS);
    set_NVM_Settings(reg1, reg2, reg3);
}

void set_uSD_Init_Settings(bool usdInit){
    global_settings.uSD_init = usdInit;
}

bool get_uSD_Init_Settings(void){
    return global_settings.uSD_init;
}

void set_PMS7003_Init_Settings(bool PMS7003_init){
    global_settings.uSD_init = PMS7003_init;
}

bool get_PMS7003_Init_Settings(void){
    return global_settings.PMS7003_init;
}

void set_SHT35_Init_Settings(bool SHT35_init){
    global_settings.SHT35_init = SHT35_init;
}

bool get_SHT35_Init_Settings(void){
    return global_settings.SHT35_init;
}

void set_LC709203F_Init_Settings(bool LC709203F_init){
    global_settings.LC709203F_init = LC709203F_init;
}

bool get_LC709203F_Init_Settings(void){
    return global_settings.LC709203F_init;
}

void set_MHET_Init_Settings(bool MHET_init){
    global_settings.MHET_init = MHET_init;
}

bool get_MHET_Init_Settings(void){
    return global_settings.MHET_init;
}

void set_ZigBee_Connected_Settings(bool ZigBee_connected){
    global_settings.ZigBee_connected = ZigBee_connected;
}

bool get_ZigBee_Connected_Settings(void){
    return global_settings.ZigBee_connected;
}

void set_Settings_Date(uint8_t hour, uint8_t minute, uint8_t month, uint16_t year){
    global_settings.hour = hour;
    global_settings.minute = minute;
    global_settings.month = month;
    global_settings.year = year;
}