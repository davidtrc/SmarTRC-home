#ifndef _PMS7003_H    /* Guard against multiple inclusion */
    #define _PMS7003_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
    extern "C" {
#endif

#define PMS_CMD_LEN         7
#define PMS_RESPONSE_SIZE   32
#define UART_ERR_TIMEOUT_MS 2

bool PMS_Init(void);
void PMS_Activation(void);    
void PMS_Deactivation(void);
bool PMS_Set_Passive_Mode(void);
bool PMS_Set_Sleep_Mode(void);
bool PMS_Wake_Up(void);
bool PMS_Passive_Read(uint16_t *pm1, uint16_t *pm25, uint16_t *pm10);
bool PMS_Wait_4_Rx(void);
bool PMS_Wait_4_Tx(void);

/* Provide C++ Compatibility */
#ifdef __cplusplus
    }
#endif

#endif /* _EXAMPLE_FILE_NAME_H */
