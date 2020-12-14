#include <math.h>
#include <stddef.h>                     // Defines NULL
#include <string.h>
#include <xc.h>

#include "definitions.h"                // SYS function prototypes
#include "SHT35.h"
#include "CustomTime.h"

void SHT_Get_Temp_and_Humidity(double *tempd, double *humid){
    uint8_t sensor_measure[2] = {0x2C, 0x06};
    uint8_t sensor_lecture[6];
    uint16_t temp, humi;
    
    memset(sensor_lecture, 0, 6);
    I2C3_Write((uint8_t)SENSOR_ADDRESS, sensor_measure, 2);
    DelayMs(50);
    I2C3_Read((uint8_t)SENSOR_ADDRESS, sensor_lecture, 6);

    temp = (sensor_lecture[0]<<8) + sensor_lecture[1];
    humi = (sensor_lecture[3]<<8) + sensor_lecture[4];

    *tempd = ((175*temp)/(pow(2,16)-1)) - 45;
    *humid = (100*humi)/(pow(2,16)-1);
}