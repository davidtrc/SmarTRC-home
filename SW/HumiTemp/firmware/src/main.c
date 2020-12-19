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
#include "debug.h"
#include "ff.h"

/**** TODO ******
 *Drivers pantalla (estudiar composicion barra superior con hora y bateria, al medio lo importante. Usar refresco parcial)
 *Modulo ZigBee
 *Recibir hora por ZigBee
 *Pensar en cosas que hacer con ZigBee
 *OTA
 *MicroSD
 */

uint8_t get_WakeUp_Cause()
{
    uint8_t int_vector = (isTmr1Expired | btn1_interrupt | btn2_interrupt | btn3_interrupt | zigbee_interrupt | sensor_interrupt);

    return int_vector;
}

bool manage_Interruptions(uint8_t wakeup_cause)
{
    
    //Checkear si se ha pulsado algun boton

    //Checkear interrupcion ZIGBEE

    //Checkear interrupcion SEN_ALERT

    //Checkear el tipo de timer (si es 35 segundos habia una medida pendiente)
    
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

static void setPeripherals_DeepSleep(bool PMS_DeepSleep)
{

    //MHET_DeepSleep();
    //ZigBee_DeepSleep();
    //uSD_DeepSleep(); Yo creo que esto no hace falta ni tiene sentido
    //SHT35_DeepSleep();
    if(PMS_DeepSleep){
        //PMS_DeepSleep();
    }
}

static void measurementRoutine(void)
{
    
    uint16_t pm1, pm25, pm10;
    double tempd, humid;
    uint16_t gauge_RSOC = 0;
    uint16_t oldTimerPeriod = 0;
    uint8_t wakeup_cause = 0;
    
    /* START OF PMS7003 initialization */
        PMS_Activation();                   //5V line activation y SET pin ENABLE
        PMS_Wake_Up();                      //Make sure the sensor is waken-up
        PMS_Set_Passive_Mode();             //Set sensor to passive mode (data send only w/request)
        oldTimerPeriod = TMR1_PeriodGet();  //Obtain old user wakeup period
        TMR1_PeriodSet(PERIOD_35S);         //Set new wakeup period (give some time to PMS sensor to warm up)
        setPeripherals_DeepSleep(false);
        OSCCONSET = 0x10;                   //Set Power-Saving mode to Sleep    
        asm volatile("wait");               //Put device in selected power-saving mode
        wakeup_cause = get_WakeUp_Cause();
        manage_Interruptions(wakeup_cause);
        
    /* END OF PMS7003 initialization */  
    PMS_Passive_Read(&pm1, &pm25, &pm10);
    PMS_Set_Sleep_Mode();   //Send PMS7003 to sleep
    PMS_Deactivation();
    
    //Checkear si se ha pulsado algun boton
    SHT_Get_Temp_and_Humidity(&tempd, &humid);
    
    gauge_RSOC = LC709203F_Get_RSOC();
    //MHET_Plot_Data(pm1, pm25, pm10, tempd, humid, gauge_RSOC);
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
    /* Initialize all modules */
    SYS_Initialize (NULL);
    
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

    printf("Starting HumiTemp\r\n");
    LED_TEST_1_Set();   //Inverted logic, turn off TEST LED 1
    LED_TEST_2_Set();   //Inverted logic, turn off TEST LEDS 2
    
    /****************************** TEST DE SD*****************************************/
    
    /* Initialize all MPLAB Harmony modules, including application(s). */
    //TRISA = 0x0000;
    //LATACLR = 0x0001;
    char buffer[256] = { 0 };
    const char text1[] = "Hello World!\r This file is created with FatFs driver!\r Have a great day!\r\r";
    unsigned int numWrite = 0;   // number of bytes written to file.
    unsigned int numRead = 0;    // number of bytes read from file. 
    
    FATFS Fatfs;
    FATFS *fs; /* Pointer to file system object */
    FIL file1;    
    
    
    // Initialize disk. If SD-card not present or faulty, wait there
    // and blink LED.
    if(disk_initialize(0)) {
        DEBUG_PRINT("No card or card error!\r");
        while(1) { 
            LED_TEST_1_Toggle();
            DelayMs(250);
        }
    }
    
    // Mounting the file system. If file system invalid, wait there and blink LED.
    if(f_mount(&Fatfs,"",0) == FR_INVALID_DRIVE) {
        DEBUG_PRINT("Invalid file system!\r");
        while(1) { 
            LED_TEST_2_Toggle();
            DelayMs(250);
        }        
    }
        
    // Create a file here named "hello123.txt":
    // (File is created if it does not exist, and if it is, file is opened instead).
    DEBUG_PRINT("Creating a file inside for reading and writing!\r");
    f_open(&file1, "hello123.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ); 

    // Write a text (contents inside text1[]) into the file.
    f_write(&file1, &text1, sizeof(text1), &numWrite);
    
    // Rewind the file pointer to the start!
    f_lseek(&file1, 0);
    
    // Read the text and store them into the buffer.
    DEBUG_PRINT("Reading the file inside...\r");
    f_read(&file1, &buffer, sizeof(text1), &numRead);
    
    // Print the buffer contents to the terminal:
    printf("Printing contents: %s\r", &buffer);
        
    // Close the file after it is done!
    printf("Closing file!\r");
    f_close(&file1);
    
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        //SYS_Tasks ( );
        LED_TEST_1_Toggle();
        LED_TEST_2_Toggle();
        DelayMs(500);
    }
    /****************************** FIN TEST DE SD*****************************************/
    
    /* START OF MH-ET LIVE initialization*/
        //MHET_Init();
        //MHET_Display_Pattern(gImage_black, gImage_red); //Welcome message
        //Leave in deep_sleep
    /* END OF MH-ET LIVE initialization*/
    
    /* START OF ZigBee initialization*/
        //ZigBee_Init();
        //ZigBee_DeepSleep();
    /* END OF ZigBee initialization*/
    
    /* START OF uSD initialization*/
        //uSD_Init();
        //uSD_DeepSleep(); Yo creo que esto no hace falta ni tiene sentido
    /* END OF uSD initialization*/
    
    /* START OF SHT35 initialization*/
        //SHT35_Init();
        //SHT35_DeepSleep();
    /* END OF SHT35 initialization*/

    /* START OF LC709203F initialization */
        LC709203F_Init();         //Must be done with everything off: ZigBee, uSD, MHET, PSM and temp/humi sensor
    /* END OF LC709203F initialization */
        
    /* START OF PMS initialization*/
        //PMS_Init();
        //Start timer to count 35 sec
    /* END OF PMS initialization*/
   
    while(true){
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks();
        
        if(!first_time){
            wakeup_cause = get_WakeUp_Cause();
            manage_Interruptions(wakeup_cause);   
        } else {
            //Measure temp, humi, RSOC
            //Update MHET loading bar (waiting for PMS 35 sec initialization) usar DelayMs(1000) para el re ploteo
            //Measure PMS
            //Plot all data in MHET
        }
        
        //MHET_Set_Time_And_Date();
        
        if(btn_test1_interrupt){
            btn_test1_interrupt = false;
            LED_TEST_1_Toggle();
        }
        
        if(btn_test2_interrupt){
            btn_test2_interrupt = false;
            LED_TEST_2_Toggle();
        }
        
        setPeripherals_DeepSleep(true);
        
        DEBUG_PRINT("Sleep Mode\r\n");
        OSCCONSET = 0x10; // set Power-Saving mode to Sleep    
        asm volatile("wait"); // put device in selected power-saving mode
    }
        
    /* Execution should not come here during normal operation */

    return (EXIT_FAILURE);
}


/*******************************************************************************
 End of File
*/

