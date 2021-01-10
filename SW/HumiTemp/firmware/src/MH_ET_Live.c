#include <math.h>
#include <stddef.h>                     // Defines NULL
#include <string.h>
#include <xc.h>

#include "definitions.h"                // SYS function prototypes
#include "MH_ET_Live.h"
#include "EPDPaint.h"
#include "CustomTime.h"
#include "debug.h"
#include "settings.h"

#define EPD_WIDTH               200
#define EPD_HEIGHT              200

unsigned char image[5000];

bool MHET_Init(void) {
    
    uint8_t DRV_OUT_CONTROL[3] = {(EPD_HEIGHT-1), ((EPD_HEIGHT - 1) >> 8), 0x01};
    uint8_t DATA_NTRY_MD[1] = {0X01}; //Counter update in X direction. Y decrement, X increment
    uint8_t RAM_X_ADDRESS_CMD[2] = {0x00, 0x18};
    uint8_t RAM_Y_ADDRESS_CMD[4] = {0xC7, 0x00, 0x00, 0x00};
    uint8_t BORDER_WAVEFORM_CMD[1] = {0X01};
    uint8_t DISPLAY_UPDATE_CONTROL_2_CMD[1] = {0XB1};
    uint8_t SET_RAM_X_COUNTER_CMD[1] = {0X00};
    uint8_t SET_RAM_Y_COUNTER_CMD[2] = {0xC7, 0X00};
    //uint8_t CMD_18_VAL[1] = {0X80};
    
    if(get_MHET_Init_Settings())
        return false;
    
    MHET_Reset();

    MHET_ReadBusy();
    MHET_Send_CMD(SW_RESET);
    MHET_ReadBusy();

    MHET_Send_CMD(DRIVER_OUTPUT_CONTROL);
    MHET_Send_Data(&DRV_OUT_CONTROL, sizeof(DRV_OUT_CONTROL));
    
    MHET_Send_CMD(DATA_ENTRY_MODE);
    MHET_Send_Data(&DATA_NTRY_MD, 1);
    
    MHET_Send_CMD(SET_RAM_X_ADDRESS_START_END_POS);
    MHET_Send_Data(&RAM_X_ADDRESS_CMD, sizeof(RAM_X_ADDRESS_CMD));

    MHET_Send_CMD(SET_RAM_Y_ADDRESS_START_END_POS);
    MHET_Send_Data(&RAM_Y_ADDRESS_CMD, sizeof(RAM_Y_ADDRESS_CMD));
    
    MHET_Send_CMD(BORDER_WAVEFORM_CONTROL);
    MHET_Send_Data(&BORDER_WAVEFORM_CMD, sizeof(BORDER_WAVEFORM_CMD));

    //MHET_Send_CMD(CMD_18);
    //MHET_Send_Data(&CMD_18_VAL, sizeof(CMD_18_VAL));

    MHET_Send_CMD(DISPLAY_UPDATE_CONTROL_2); //Load Temperature and waveform setting.
    MHET_Send_Data(&DISPLAY_UPDATE_CONTROL_2_CMD, sizeof(DISPLAY_UPDATE_CONTROL_2_CMD));
    
    MHET_Send_CMD(MASTER_ACTIVATION);

    MHET_Send_CMD(SET_RAM_X_COUNTER); //set RAM x address count to 0;
    MHET_Send_Data(&SET_RAM_X_COUNTER_CMD, sizeof(SET_RAM_X_COUNTER_CMD));
    
    MHET_Send_CMD(SET_RAM_Y_COUNTER); //set RAM y address count to 0X199;
    MHET_Send_Data(&SET_RAM_Y_COUNTER_CMD, sizeof(SET_RAM_Y_COUNTER_CMD));

    MHET_ReadBusy();
    
    Paint_Init(image, 0, 0);
    
    return true;
}

void MHET_Reset(void){
    MHET_RST_Set();
    DelayMs(200);
    MHET_RST_Clear();
    DelayMs(5);
    MHET_RST_Set();
    DelayMs(200);
}

bool MHET_Send_CMD(uint8_t TransmitData){
    
    uint8_t pTransmitData[1] = {TransmitData};
    bool ret;
            
    if(MHET_DC_Get() == 1){
        MHET_DC_Toggle();
    }
    ret = SPI2_Write(pTransmitData, CMD_SIZE);
    if(MHET_DC_Get() == 0){
        MHET_DC_Toggle();
    }
    
    return ret;
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

void MHET_ReadBusy(void){    
    uint32_t tWait = ( GetSystemClock() / 2000 ) * READBUSY_TIMEOUT_MS; //Delay for timeout
    uint32_t tStart = ReadCoreTimer(); 
    uint32_t timeElapsed = 0;
    
    while( (MHET_BUSY_Get() == 0) && (timeElapsed < tWait)) {    //LOW: busy, HIGH: idle
        timeElapsed = ReadCoreTimer() - tStart;
    }      
    if(timeElapsed>=tWait){
        printf("MH-ET BUSY NOT RELEASED IN %d MS\r", tWait);
    }
}

void MHET_TurnOnDisplay(void){
    uint8_t cmd[1] = {0xF7};
    
    MHET_Send_CMD(0x22);                //DISPLAY_UPDATE_CONTROL_2
    MHET_Send_Data(&cmd[0], 1);
    MHET_Send_CMD(0x20);                //MASTER_ACTIVATION
        
    MHET_ReadBusy();
}

void MHET_TurnOnDisplayPart(void){
    uint8_t cmd[1] = {0xFF};
    
    MHET_Send_CMD(0x22);                //DISPLAY_UPDATE_CONTROL_2
    MHET_Send_Data(&cmd[0], 1);
    MHET_Send_CMD(0x20);                //MASTER_ACTIVATION
        
    MHET_ReadBusy();
}

void MHET_Clear(uint8_t color){
    
    uint8_t i;
    uint8_t j;
    
    uint8_t data[1] = {color};
    
    MHET_Send_CMD(0x24);
    
    for(j=0; j<EPD_HEIGHT; j++){
        for(i=0; i<(uint8_t)(EPD_WIDTH/8); i++){
            MHET_Send_Data(&data[0], 1);
        }
    }
    
    MHET_TurnOnDisplay();
}


/*  
void MHET_GetBuffer(uint8_t *image){
    
}      
    def getbuffer(self, image):
        buf = [0xFF] * (int(self.width/8) * self.height)
        image_monocolor = image.convert('1')
        imwidth, imheight = image_monocolor.size
        pixels = image_monocolor.load()
        if(imwidth == self.width and imheight == self.height):
            logging.debug("Horizontal")
            for y in range(imheight):
                for x in range(imwidth):
                    # Set the bits for the column of pixels at the current position.
                    if pixels[x, y] == 0:
                        buf[int((x + y * self.width) / 8)] &= ~(0x80 >> (x % 8))
        elif(imwidth == self.height and imheight == self.width):
            logging.debug("Vertical")
            for y in range(imheight):
                for x in range(imwidth):
                    newx = y
                    newy = self.height - x - 1
                    if pixels[x, y] == 0:
                        buf[int((newx + newy*self.width) / 8)] &= ~(0x80 >> (y % 8))
        return buf
*/

void MHET_Display(void){
    uint8_t i = 0;
    uint8_t j = 0;
    
    MHET_Send_CMD(MHET_WRITE_RAM);
    for(j=0; j<EPD_HEIGHT; j++){
        for(i=0; i<(uint8_t)(EPD_WIDTH/8); i++){
            MHET_Send_Data(&image[i + j * (uint8_t)(EPD_WIDTH/8)], 1);
        }
    }
    
    MHET_TurnOnDisplay();
}

void MHET_DisplayPartBaseImage(void){
    uint8_t i = 0;
    uint8_t j = 0;
    
    MHET_Send_CMD(MHET_WRITE_RAM);
    for(j=0; j<EPD_HEIGHT; j++){
        for(i=0; i<(uint8_t)(EPD_WIDTH/8); i++){
            MHET_Send_Data(&image[i + j * (uint8_t)(EPD_WIDTH/8)], 1);
        }
    }
    
    MHET_Send_CMD(MHET_CMD_26);
    for(j=0; j<EPD_HEIGHT; j++){
        for(i=0; i<(uint8_t)(EPD_WIDTH/8); i++){
            MHET_Send_Data(&image[i + j * (uint8_t)(EPD_WIDTH/8)], 1);
        }
    }    

    MHET_TurnOnDisplayPart();
}

void MHET_DisplayPart(void){
    
    uint8_t data[1] = {0x80};
    uint8_t i = 0;
    uint8_t j = 0;
    
    MHET_RST_Clear();
    DelayMs(10);
    MHET_RST_Set();
    DelayMs(10);
    MHET_RST_Set();
    DelayMs(200);
    
    MHET_Send_CMD(BORDER_WAVEFORM_CONTROL);
    MHET_Send_Data(&data[0], 1);
    
    MHET_Send_CMD(MHET_WRITE_RAM);
    for(j=0; j<EPD_HEIGHT; j++){
        for(i=0; i<(uint8_t)(EPD_WIDTH/8); i++){
           MHET_Send_Data(&image[i + j * (uint8_t)(EPD_WIDTH/8)], 1); 
        }
    }
    
    MHET_TurnOnDisplayPart();
}

void MHET_Sleep(void){
    uint8_t data[1] = {0x01};
    
    MHET_Send_CMD(MHET_DEEP_SLEEP);
    MHET_Send_Data(&data[0], 1);
}

void MHET_WakeFromSleep(void){
    uint8_t data[1] = {0x00};
    
    MHET_Send_CMD(MHET_DEEP_SLEEP);
    MHET_Send_Data(&data[0], 1);  
}

void MHET_Display_Welcome_Message(void){
    
}

void MHET_Set_Time_And_Date(void){
    
}

void MHET_Plot_Data(uint16_t pm1, uint16_t pm25, uint16_t pm10, double tempd, double humid, uint16_t gauge_RSOC){
    DEBUG_PRINT(" pm1=%d, pm25=%d, pm10=%d, temp=%f, humi=%f, RSOC=%u", pm1, pm25, pm10, tempd, humid, gauge_RSOC);
    DelayMs(1000);
}

void MHET_Update_ProgressBar(void){
    static uint8_t counter = 0;
    
    DEBUG_PRINT("Counter =%d", counter);
    counter ++;
}