#ifndef _SHT35_H    /* Guard against multiple inclusion */
    #define _SHT35_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
    extern "C" {
#endif

#define SENSOR_ADDRESS          0x44
#define HIGH_REPETEABILITY      0
#define MEDIUM_REPETEABILITY    1
#define LOW_REPETEABILITY       2
#define MPS_05                  0
#define MPS_1                   1
#define MPS_2                   2
#define MPS_4                   4
#define MPS_10                  10

bool SHT_Init(void);        
        
bool SHT_Get_Temp_and_Humidity(double *tempd, double *humid);

bool SHT_SoftReset(void);

bool SHT_Set_Single_Shot_Mode(bool clock_stretching, uint8_t repeteability);

bool SHT_Set_Periodic_Shot_Mode(uint8_t measures_per_second, uint8_t repeteability);

bool SHT_Stop_Periodic_Shot_Mode(void);

bool SHT_Get_Status_Register(uint16_t *reg_val);

bool SHT_Clear_Status_Register(void);

bool SHT_Enable_Alert_Mode(uint16_t temp_HI_SET, uint16_t temp_HI_CLEAR, uint16_t temp_LO_SET, uint16_t temp_LO_CLEAR, 
                           uint16_t humi_HI_SET, uint16_t humi_HI_CLEAR, uint16_t humi_LO_SET, uint16_t humi_LO_CLEAR);

bool SHT_Get_Alert_Thresholds(uint16_t *temp_HI_SET, uint16_t *temp_HI_CLEAR, uint16_t *temp_LO_SET, uint16_t *temp_LO_CLEAR, 
                              uint16_t *humi_HI_SET, uint16_t *humi_HI_CLEAR, uint16_t *humi_LO_SET, uint16_t *humi_LO_CLEAR);

bool SHT_Disable_Alert_Mode(void);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */
