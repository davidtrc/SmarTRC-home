#include <math.h>
#include <stddef.h>                     // Defines NULL
#include <string.h>
#include <xc.h>

#include "definitions.h"                // SYS function prototypes
#include "MH_ET_Live.h"
#include "CustomTime.h"
#include "debug.h"

int MHET_Init(void) {
    
    uint8_t POWER_SETTING_CMD[4] = {0x07, 0x00, 0x08, 0x00};
    uint8_t BOOSTER_CMD[3] = {0x07, 0x07, 0x07};
    uint8_t TCON_RES_CMD[3] = {WIDTH, (HEIGHT>>8), HEIGHT};
   
    MHET_Reset();
    
    MHET_Send_CMD(POWER_SETTING);
    MHET_Send_Data(POWER_SETTING_CMD, 4);

    MHET_Send_CMD(BOOSTER_SOFT_START);
    MHET_Send_Data(BOOSTER_CMD, 3);
    
    MHET_Send_CMD(POWER_ON);
    //printf("1\r\n");
    MHET_Wait_Until_Idle();
    //printf("2\r\n");
    MHET_Send_CMD(PANEL_SETTING);
    MHET_Send_Byte(0xCF);

    MHET_Send_CMD(VCOM_AND_DATA_INTERVAL_SETTING);
    MHET_Send_Byte(0x17);

    MHET_Send_CMD(PLL_CONTROL);
    MHET_Send_Byte(0x39);

    MHET_Send_CMD(TCON_RESOLUTION);
    MHET_Send_Data(TCON_RES_CMD, 3);

    MHET_Send_CMD(VCM_DC_SETTING_REGISTER);
    MHET_Send_Byte(0x30);//0E

    MHET_Set_Black_LookUp_Tables();
    MHET_Set_Red_LookUp_Tables();
    //printf("3\r\n");
    return 0;
}

void MHET_Reset(){
    
    if(MHET_RST_Get() == 1){
        MHET_RST_Toggle();
    }
    
    DelayMs(3000);
    
    if(MHET_RST_Get() == 0){
        MHET_RST_Toggle();
    }
    
    DelayMs(200);
}

bool MHET_Send_CMD(uint8_t TransmitData){
    
    uint8_t pTransmitData[1] = {TransmitData};
            
    if(MHET_DC_Get() == 1){
        MHET_DC_Toggle();
    }
    return (SPI2_Write(pTransmitData, CMD_SIZE));
}

bool MHET_Send_Data(void* pTransmitData, size_t txSize){
    if(MHET_DC_Get() == 0){
        MHET_DC_Toggle();
    }
    return (SPI2_Write(pTransmitData, txSize));
}

bool MHET_Send_Byte(uint8_t TransmitData){
    
    uint8_t pTransmitData[1] = {TransmitData};
    
    if(MHET_DC_Get() == 0){
        MHET_DC_Toggle();
    }
    return (SPI2_Write(pTransmitData, 1));
}

void MHET_Wait_Until_Idle(void){
    while(MHET_BUSY_Get() == 0) {    //LOW: busy, HIGH: idle
        DelayMs(100);
    }      
}

void MHET_Set_Black_LookUp_Tables(void) {

    uint8_t lut_vcom0[] = { 0x0E, 0x14, 0x01, 0x0A, 0x06, 0x04, 0x0A, 0x0A, 0x0F, 0x03, 0x03, 0x0C, 0x06, 0x0A, 0x00 };
    uint8_t lut_w[]     = { 0x0E, 0x14, 0x01, 0x0A, 0x46, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x86, 0x0A, 0x04 };
    uint8_t lut_b[]     = { 0x0E, 0x14, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x4A, 0x04 };
    uint8_t lut_g1[]    = { 0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04 };
    uint8_t lut_g2[]    = { 0x8E, 0x94, 0x01, 0x8A, 0x06, 0x04, 0x8A, 0x4A, 0x0F, 0x83, 0x43, 0x0C, 0x06, 0x0A, 0x04 };

    MHET_Send_CMD(0x20);         //g vcom
    MHET_Send_Data(lut_vcom0, 15);
    
    MHET_Send_CMD(0x21);         //g ww --
    MHET_Send_Data(lut_w, 15);
    
    MHET_Send_CMD(0x22);         //g bw r
    MHET_Send_Data(lut_b, 15);
    
    MHET_Send_CMD(0x23);         //g wb w
    MHET_Send_Data(lut_g1, 15);
    
    MHET_Send_CMD(0x24);         //g bb b
    MHET_Send_Data(lut_g2, 15);
    
}

void MHET_Set_Red_LookUp_Tables(void) {

    uint8_t lut_vcom1[] = { 0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8_t lut_red0[]  = { 0x83, 0x5D, 0x01, 0x81, 0x48, 0x23, 0x77, 0x77, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8_t lut_red1[]  = { 0x03, 0x1D, 0x01, 0x01, 0x08, 0x23, 0x37, 0x37, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    MHET_Send_CMD(0x25);
    MHET_Send_Data(lut_vcom1, 15);

    MHET_Send_CMD(0x26);
    MHET_Send_Data(lut_red0, 15);
        
    MHET_Send_CMD(0x27);
    MHET_Send_Data(lut_red1, 15);
}

/**
 *  @brief: After this command is transmitted, the chip would enter the 
 *          deep-sleep mode to save power. 
 *          The deep sleep mode would return to standby by hardware reset. 
 *          The only one parameter is a check code, the command would be
 *          executed if check code = 0xA5. 
 *          You can use UC8154c_200x200::Init() to awaken
 */
void MHET_Sleep() {
    
    uint8_t POWER_CMD[4] = {0x02, 0x00, 0x00, 0x00};
    
    MHET_Send_CMD(VCOM_AND_DATA_INTERVAL_SETTING);
    MHET_Send_Byte(0x17);
    
    MHET_Send_CMD(VCM_DC_SETTING_REGISTER);         //to solve Vcom drop
    MHET_Send_Byte(0x00);
    
    MHET_Send_CMD(POWER_SETTING);         //power setting
    MHET_Send_Data(POWER_CMD, 4);        //gate switch to external
    
    MHET_Wait_Until_Idle();
    
    MHET_Send_CMD(POWER_OFF);         //power off
}

void MHET_Display_Pattern(uint8_t* pattern_buffer_black, uint8_t* pattern_buffer_red) {

    uint8_t temp;
    int i = 0;
    int bit = 0;
    printf("0\r\n");
    if (pattern_buffer_black != NULL) {
        MHET_Send_CMD(DATA_START_TRANSMISSION_1);
        DelayMs(2);
        for (i = 0; i < WIDTH * HEIGHT / 8; i++) {
            temp = 0x00;
            for (bit = 0; bit < 4; bit++) {
                if ((pattern_buffer_black[i] & (0x80 >> bit)) != 0) {
                    temp |= 0xC0 >> (bit * 2);
                }
            }
            MHET_Send_Byte(temp);
            temp = 0x00;
            for (bit = 4; bit < 8; bit++) {
                if ((pattern_buffer_black[i] & (0x80 >> bit)) != 0) {
                    temp |= 0xC0 >> ((bit - 4) * 2);
                }
            }
            MHET_Send_Byte(temp);
        }
        DelayMs(2);
    }
    printf("1\r\n");
    if (pattern_buffer_red != NULL) {
        MHET_Send_CMD(DATA_START_TRANSMISSION_2);
        DelayMs(2);
        for (i = 0; i < WIDTH * HEIGHT / 8; i++) {
            MHET_Send_Byte(pattern_buffer_red[i]);
        }
        DelayMs(2);
    }
    MHET_Send_CMD(DISPLAY_REFRESH);
    printf("2\r\n");
    MHET_Wait_Until_Idle();
    printf("3\r\n");
}

void MHET_Set_Time_And_Date(void){
    
}

void MHET_Plot_Data(settings_t *global_settings, uint16_t pm1, uint16_t pm25, uint16_t pm10, double tempd, double humid, uint16_t gauge_RSOC){
    DEBUG_PRINT(" pm1=%d, pm25=%d, pm10=%d, temp=%f, humi=%f, RSOC=%u", pm1, pm25, pm10, tempd, humid, gauge_RSOC);
    DelayMs(1000);
}

void MHET_Update_ProgressBar(void){
    static uint8_t counter = 0;
    
    DEBUG_PRINT("Counter =%d", counter);
    counter ++;
}