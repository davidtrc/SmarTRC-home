/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _SETTINGS_H    /* Guard against multiple inclusion */
    #define _SETTINGS_H

#include "config/default/peripheral/nvm/plib_nvm.h"

#define SETTINGS_START_ADDRESS              (NVM_FLASH_START_ADDRESS + NVM_FLASH_PAGESIZE)

#define SETTINGS_FIRST_BOOT_ADDRESS         SETTINGS_START_ADDRESS
#define SETTINGS_FIRST_BOOT_PATTERN         0x19101993
#define SETTINGS_WAKEUP_TIME_ADDRESS        SETTINGS_START_ADDRESS+4
#define SETTINGS_WAKEUP_TIME_DEFAULT        5
#define SETTINGS_HEATING_TEMP_ADDRESS       SETTINGS_START_ADDRESS+8
#define SETTINGS_HEATING_TEMP_DEFAULT       195
#define SETTINGS_ZIGBEE_MASTERID_ADDRESS    SETTINGS_START_ADDRESS+12
#define SETTINGS_ZIGBEE_MASTERID_DEFAULT    1234

#define TOTAL_SETTINGS                      4 //First boot, wakeup time, heating temp, zigbee master ID

typedef struct _settings_t {
    uint8_t wakeup_time;
    double heating_temp;
    uint16_t ZigBee_MasterID;
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t hour;
    uint8_t minute;
    bool SHT35_init;
    bool ZigBee_init;
    bool MHET_init;
    bool PMS7003_init;
    bool uSD_init;
    bool LC709203F_init;
    bool ZigBee_connected;
} settings_t;


bool is_Settings_First_Boot(void);

void load_NVM_Settings(void);

void set_NVM_Settings(uint32_t WAKEUP_TIME, uint32_t HEATING_TEMP, uint32_t ZIGBEE_MASTERID);

void set_Default_NVM_Settings(void);

void set_Default_Settings(void);

uint8_t get_NVM_WakeUp_Time(void);

void set_NVM_WakeUp_Time(uint8_t new_time);

double get_NVM_Heating_Temp(void);

void set_NVM_Heating_Temp(double new_temp);

uint16_t get_NVM_ZigBee_MasterID(void);

void set_NVM_ZigBee_MasterID(uint16_t new_ID);

void set_uSD_Init_Settings(bool usdInit);

bool get_uSD_Init_Settings(void);

void set_PMS7003_Init_Settings(bool PMS7003_init);

bool get_PMS7003_Init_Settings(void);

void set_SHT35_Init_Settings(bool SHT35_init);

bool get_SHT35_Init_Settings(void);

void set_LC709203F_Init_Settings(bool LC709203F_init);

bool get_LC709203F_Init_Settings(void);

void set_MHET_Init_Settings(bool MHET_init);

bool get_MHET_Init_Settings(void);

void set_ZigBee_Connected_Settings(bool ZigBee_connected);

bool get_ZigBee_Connected_Settings(void);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
