#ifndef _PMS7003_H    /* Guard against multiple inclusion */
    #define _PMS7003_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
    extern "C" {
#endif

#define PMS_CMD_LEN         7
#define PMS_RESPONSE_SIZE   32

void PMS_Activation();    
void PMS_Deactivation();
void PMS_Set_Passive_Mode();
void PMS_Set_Sleep_Mode();
void PMS_Wake_Up();
void PMS_Passive_Read(uint16_t *pm1, uint16_t *pm25, uint16_t *pm10);

/* Provide C++ Compatibility */
#ifdef __cplusplus
    }
#endif

#endif /* _EXAMPLE_FILE_NAME_H */
