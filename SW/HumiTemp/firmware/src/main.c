/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

/*
 * LC709203F -> I2C1
 * SHT35 -> I2C3
 * ZIGBEE -> SPI1
 * MH_ET -> SPI2
 * MICROSD -> SPI3
 * PMS7003 -> UART3
 * STDIO -> UART2
 */

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdio.h>
#include <string.h>
#include <xc.h>
#include <math.h>

#include "definitions.h"                // SYS function prototypes
#include "PMS7003.h"
#include "SHT35.h"
#include "MH_ET_Live.h"
#include "CustomTime.h"
#include "LC709203F.h"
#include "ZigBee.h"
#include "uSD.h"
#include "debug.h"
#include "diskio.h"
#include "ff.h"

settings_t global_settings;
/**** TODO ******
 *Drivers pantalla (estudiar composicion barra superior con hora y bateria, al medio lo importante. Usar refresco parcial)
 *Modulo ZigBee
 *  Recibir hora por ZigBee
 *  Pensar en cosas que hacer con ZigBee
 *OTA
 *MicroSD
 */

static void setPeripherals_DeepSleep(bool PMS_DeepSleep);

uint8_t get_WakeUp_Cause()
{
    uint8_t int_vector = (isTmr1Expired | btn1_interrupt | btn2_interrupt | btn3_interrupt | zigbee_interrupt | sensor_interrupt);

    return int_vector;
}

void clear_WakeUp_Cause(bool clearTimer, bool clearBtn1, bool clearBtn2, bool clearBtn3, bool clearZigBee, bool clear_sensor)
{
    if(clearTimer)
        isTmr1Expired = false;
    if(clearBtn1)
        btn1_interrupt = false;
    if(clearBtn2)
        btn2_interrupt = false;
    if(clearBtn3)
        btn3_interrupt = false;
    if(clearZigBee)
        zigbee_interrupt = false;
    if(clear_sensor)
        sensor_interrupt = false;
}

void check_Initializations(void){
    
}

static void measurementRoutine(void)
{
    
    uint16_t pm1, pm25, pm10;
    double tempd, humid;
    uint16_t gauge_RSOC = 0;
    //uint16_t oldTimerPeriod = 0;
    //uint8_t wakeup_cause = 0;
    
    /* START OF PMS7003 initialization */
        PMS_Activation();                   //5V line activation y SET pin ENABLE
        PMS_Wake_Up();                      //Make sure the sensor is waken-up
        PMS_Set_Passive_Mode();             //Set sensor to passive mode (data send only w/request)
        //oldTimerPeriod = TMR1_PeriodGet();  //Obtain old user wakeup period
        TMR1_PeriodSet(PERIOD_35S);         //Set new wakeup period (give some time to PMS sensor to warm up)
        setPeripherals_DeepSleep(false);
    /* END OF PMS7003 initialization */  
    PMS_Passive_Read(&pm1, &pm25, &pm10);
    PMS_Set_Sleep_Mode();   //Send PMS7003 to sleep
    PMS_Deactivation();
    
    //Checkear si se ha pulsado algun boton
    SHT_Get_Temp_and_Humidity(&tempd, &humid);
    
    gauge_RSOC = LC709203F_Get_RSOC();
    MHET_Plot_Data(&global_settings, pm1, pm25, pm10, tempd, humid, gauge_RSOC);
}

static void setPeripherals_DeepSleep(bool PMS_DeepSleep)
{
    //MHET_DeepSleep();
    //ZigBee_DeepSleep();
    if(PMS_DeepSleep){
        measurementRoutine(); //Just for avoid compilation errors, remove it
        //PMS_DeepSleep();
    }
}

bool manage_Interruptions(uint8_t wakeup_cause)
{
    
    if(wakeup_cause & TIMER1_INT_GENERATED){
        //Analizar si se int de 1 min o la de 1 min*x = tiempo de medidas fijado por usuario
        //El tiempo del usuario debería guardarse en la SD en un archivo de conf
        //measurementRoutine();
    }
    
    if(wakeup_cause & BTN1_INT_GENERATED){
        //Analizar si se int de 1 min o la de 1 min*x = tiempo de medidas fijado por usuario
        //El tiempo del usuario debería guardarse en la SD en un archivo de conf
    }
    
    if(wakeup_cause & BTN2_INT_GENERATED){
        //Analizar si se int de 1 min o la de 1 min*x = tiempo de medidas fijado por usuario
        //El tiempo del usuario debería guardarse en la SD en un archivo de conf
    }
    
    if(wakeup_cause & BTN3_INT_GENERATED){
        //Analizar si se int de 1 min o la de 1 min*x = tiempo de medidas fijado por usuario
        //El tiempo del usuario debería guardarse en la SD en un archivo de conf
    }
    
    if(wakeup_cause & ZIGBEE_INT_GENERATED){
        //Analizar si se int de 1 min o la de 1 min*x = tiempo de medidas fijado por usuario
        //El tiempo del usuario debería guardarse en la SD en un archivo de conf
    }
    
    if(wakeup_cause & SENSOR_INT_GENERATED){

    }
    
    return true;
}

static void SW1_User_Handler(GPIO_PIN pin, uintptr_t context)
{
    if(BTN_TST_1_Get() == 0)
    {
        btn_test1_interrupt = true;      
    }
}

static void SW2_User_Handler(GPIO_PIN pin, uintptr_t context)
{
    if(BTN_TST_2_Get() == 0)
    {
        btn_test2_interrupt = true;      
    }
}

static void tmr1EventHandler(uint32_t intCause, uintptr_t context)
{
    isTmr1Expired = TIMER1_INT_GENERATED;                              
}

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
int main (void)
{
    __XC_UART = 2; 
    uint8_t wakeup_cause = 0;
    bool first_time = true;
    uint32_t tStart = 0;
    uint16_t gauge_RSOC = 0;
    uint16_t pm1, pm25, pm10 = 0;
    double tempd, humid = 0;
    
    uint8_t LC709203F_init = 0;
       
    /* Initialize all modules */
    SYS_Initialize (NULL);
    
    printf("Starting HumiTemp\r\n");
    
    /* Enable test buttons interruptions and callbacks */
    GPIO_PinInterruptCallbackRegister(BTN_TST_1_PIN, SW1_User_Handler, 0);
    GPIO_PinInterruptCallbackRegister(BTN_TST_2_PIN, SW2_User_Handler, 0);
    GPIO_PinInterruptEnable(BTN_TST_1_PIN);
    GPIO_PinInterruptEnable(BTN_TST_2_PIN);
    
    /* Start the timer 1 and callbacks */
    TMR1_CallbackRegister(tmr1EventHandler, 0);
    TMR1_Start();
    
    /* Enable the three buttons as INT sources */
    EVIC_SourceEnable(INT_SOURCE_EXTERNAL_2);
    EVIC_SourceEnable(INT_SOURCE_EXTERNAL_3);
    EVIC_SourceEnable(INT_SOURCE_EXTERNAL_4);

    LED_TEST_1_Set();   //Inverted logic, turn off TEST LED 1
    LED_TEST_2_Set();   //Inverted logic, turn off TEST LEDS 2
    
    /* START MH-ET LIVE initialization*/
        //global_settings.MHET_init = MHET_Init();
        //if(global_settings.PMS7003_init)
            //MHET_Display_Pattern(gImage_black, gImage_red); //Welcome message
        //Leave in deep_sleep
    /* END MH-ET LIVE initialization*/
    
    /* START OF PMS initialization*/
        global_settings.PMS7003_init = PMS_Init();
        if(global_settings.PMS7003_init)
            tStart = ReadCoreTimer();     //Start timer to count 35 sec
    /* END OF PMS initialization*/
    
    /* START OF uSD initialization*/
        global_settings.uSD_init = init_uSD();
    /* END uSD initialization*/
    
    /* START OF ZigBee initialization*/
        ZIGBEE_RST_Set();
        ZIGBEE_WAKE_Set();
        DelayMs(500);
        uint8_t zigbee_reg = ZigBee_Get_Short_Register_Value(ZIGBEE_TXMCR_REG);
        DEBUG_PRINT("Zigbee_reg=%d", zigbee_reg);
        //ZigBee_Init();
        //ZigBee_DeepSleep();
    /* END ZigBee initialization*/
    
    /* START OF SHT35 initialization*/
        global_settings.SHT35_init = SHT_Init();
    /* END OF SHT35 initialization*/
    
    /* START OF LC709203F initialization */
        LC709203F_init = LC709203F_Init();         //Must be done with everything off: ZigBee, uSD, MHET, PSM and temp/humi sensor
        if(!LC709203F_init)
            global_settings.LC709203F_init = true;
    /* END OF LC709203F initialization */
   
    while(true){
        // Maintain state machines of all polled MPLAB Harmony modules. 
        SYS_Tasks();
        
        if(first_time){
            Load_SD_DDBB(&global_settings);
            uint32_t tWait = ( GetSystemClock() / 2000 ) * PMS_MEASURE_WAIT_MS;
            if(global_settings.MHET_init){
                if(global_settings.PMS7003_init){
                    while( ( ReadCoreTimer() - tStart ) < tWait ){
                        DelayMs(1000);
                        MHET_Update_ProgressBar();
                    }     
                }
            }
            if(global_settings.PMS7003_init){
                PMS_Passive_Read(&pm1, &pm25, &pm10);
                PMS_Set_Sleep_Mode();
            }
            PMS_Deactivation();
            if(global_settings.SHT35_init)
                SHT_Get_Temp_and_Humidity(&tempd, &humid);                          //Get initial measures
            if(global_settings.LC709203F_init)
                gauge_RSOC = LC709203F_Get_RSOC();
            if(global_settings.MHET_init)
                MHET_Plot_Data(&global_settings, pm1, pm25, pm10, tempd, humid, gauge_RSOC);
            
            first_time = false;
            TMR1_CounterReset();                                                //Reset wakeup timer
            clear_WakeUp_Cause(true, true, true, true, true, true);             //Clear interruption vector variables
        } else {
            check_Initializations();
            wakeup_cause = get_WakeUp_Cause();
            manage_Interruptions(wakeup_cause);   
            
            ZIGBEE_RST_Set();
            ZIGBEE_WAKE_Set();
            DelayMs(500);
            uint8_t zigbee_reg = ZigBee_Get_Short_Register_Value(ZIGBEE_TXMCR_REG);
            DEBUG_PRINT("Zigbee_reg=%d", zigbee_reg);
        }
        
        if(global_settings.MHET_init)
            MHET_Set_Time_And_Date();
        
        DEBUG_PRINT("Sleep Mode\r\n");
        DelayMs(50);
        OSCCONSET = 0x10;                                                   //Set Power-Saving mode to Sleep    
        asm volatile("wait");                                               //Put device in selected power-saving mode 
    }
        
    /* Execution should not come here during normal operation */

    return (EXIT_FAILURE);
}


/*******************************************************************************
 End of File
*/

