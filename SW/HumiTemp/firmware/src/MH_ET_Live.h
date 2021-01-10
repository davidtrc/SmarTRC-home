#ifndef _MH_ET_LIVE_H    /* Guard against multiple inclusion */
    #define _MH_ET_LIVE_H

#include <stddef.h>                     // Defines NULL
#include <string.h>
#include <xc.h>

#include "definitions.h"                // SYS function prototypes
#include "settings.h"

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif
    
#define CMD_SIZE    1

//commands list
#define DRIVER_OUTPUT_CONTROL                       0x01
#define MHET_DEEP_SLEEP                             0x10    
#define DATA_ENTRY_MODE                             0x11    
#define SW_RESET                                    0x12
#define CMD_18                                      0x18
#define MASTER_ACTIVATION                           0x20
#define DISPLAY_UPDATE_CONTROL_2                    0x22
#define MHET_WRITE_RAM                              0x24
#define MHET_CMD_26                                 0x26
#define BORDER_WAVEFORM_CONTROL                     0X3C
#define SET_RAM_X_COUNTER                           0X4E
#define SET_RAM_Y_COUNTER                           0X4F
#define SET_RAM_X_ADDRESS_START_END_POS             0X44
#define SET_RAM_Y_ADDRESS_START_END_POS             0X45
    
#define READBUSY_TIMEOUT_MS                         3000    
 
bool MHET_Init(void);
void MHET_Reset(void);
bool MHET_Send_CMD(uint8_t pTransmitData);
bool MHET_Send_Data(void* pTransmitData, size_t txSize);
bool MHET_Send_Byte(uint8_t TransmitData);
void MHET_ReadBusy(void);
void MHET_TurnOnDisplay(void);
void MHET_TurnOnDisplayPart(void);
void MHET_Clear(uint8_t color);
void MHET_Display(void);
void MHET_DisplayPartBaseImage(void);
void MHET_DisplayPart(void);
void MHET_Sleep();
void MHET_WakeFromSleep(void);
void MHET_Display_Welcome_Message(void);
void MHET_Set_Time_And_Date(void);
void MHET_Plot_Data(uint16_t pm1, uint16_t pm25, uint16_t pm10, double tempd, double humid, uint16_t gauge_RSOC);
void MHET_Update_ProgressBar(void);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */
