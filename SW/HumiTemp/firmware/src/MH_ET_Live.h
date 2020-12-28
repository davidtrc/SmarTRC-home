#ifndef _MH_ET_LIVE_H    /* Guard against multiple inclusion */
    #define _MH_ET_LIVE_H

#include <stddef.h>                     // Defines NULL
#include <string.h>
#include <xc.h>

#include "definitions.h"                // SYS function prototypes

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#define WIDTH       200
#define HEIGHT      200
    
#define CMD_SIZE    1

//commands list
#define PANEL_SETTING                               0x00
#define POWER_SETTING                               0x01
#define POWER_OFF                                   0x02
#define POWER_OFF_SEQUENCE_SETTING                  0x03
#define POWER_ON                                    0x04
#define POWER_ON_MEASURE                            0x05
#define BOOSTER_SOFT_START                          0x06
#define DEEP_SLEEP                                  0x07
#define DATA_START_TRANSMISSION_1                   0x10
#define DATA_STOP                                   0x11
#define DISPLAY_REFRESH                             0x12
#define DATA_START_TRANSMISSION_2                   0x13
#define PLL_CONTROL                                 0x30
#define TEMPERATURE_SENSOR_COMMAND                  0x40
#define TEMPERATURE_SENSOR_CALIBRATION              0x41
#define TEMPERATURE_SENSOR_WRITE                    0x42
#define TEMPERATURE_SENSOR_READ                     0x43
#define VCOM_AND_DATA_INTERVAL_SETTING              0x50
#define LOW_POWER_DETECTION                         0x51
#define TCON_SETTING                                0x60
#define TCON_RESOLUTION                             0x61
#define SOURCE_AND_GATE_START_SETTING               0x62
#define GET_STATUS                                  0x71
#define AUTO_MEASURE_VCOM                           0x80
#define VCOM_VALUE                                  0x81
#define VCM_DC_SETTING_REGISTER                     0x82
#define PROGRAM_MODE                                0xA0
#define ACTIVE_PROGRAM                              0xA1
#define READ_OTP_DATA                               0xA2

#define Lut_length 15


int MHET_Init(void);
void MHET_Reset();
bool MHET_Send_CMD(uint8_t pTransmitData);
bool MHET_Send_Data(void* pTransmitData, size_t txSize);
bool MHET_Send_Byte(uint8_t TransmitData);
void MHET_Wait_Until_Idle(void);
void MHET_Set_Black_LookUp_Tables(void);
void MHET_Set_Red_LookUp_Tables(void);
void MHET_Sleep();
void MHET_Display_Pattern(uint8_t* pattern_buffer_black, uint8_t* pattern_buffer_red);
void MHET_Set_Time_And_Date(void);
void MHET_Plot_Data(settings_t *global_settings, uint16_t pm1, uint16_t pm25, uint16_t pm10, double tempd, double humid, uint16_t gauge_RSOC);
void MHET_Update_ProgressBar(void);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */
