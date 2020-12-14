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

/**** TODO ******
 *Drivers sensor bateria
 *Drivers pantalla (estudiar composicion barra superior con hora y bateria, al medio lo importante. Usar refresco parcial)
 *Interrupciones de botones
 *Wakeup con interrupciones de botones o timeout
 *Modulo ZigBee
 *Recibir hora por ZigBee
 *Pensar en cosas que hacer con ZigBee
 *OTA
 *MicroSD
 
 
 
 */

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************
int main ( void )
{
    __XC_UART = 2; 
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    
    uint16_t pm1, pm25, pm10;
    uint16_t voltage = 0;
    
    //uint8_t gauge_lecture[5];
    //uint8_t gauge_id_cmd[1] = {0x09};
    //size_t lengauge = 5;
    //size_t wrcmd = 1;
    
    double tempd, humid;

    printf("Hello World\r\n");
    
    /* START OF LC709203F initialization */
        DelayMs(2000);                      //Delay to let the uC to drop current consumption
        LC709203F_Init();         
    /* END OF LC709203F initialization */  
    
    /* START OF PMS7003 initialization */
        //PMS_Activation();                   //5V line activation y SET pin ENABLE
        //PMS_Wake_Up();                      //Make sure the sensor is waken-up
        //PMS_Set_Passive_Mode();             //Set sensor to passive mode (data send only w/request)
    /* END OF PMS7003 initialization */  
        
    /* START OF MH-ET LIVE initialization*/
        //MHET_Init();
        //MHET_Display_Pattern(gImage_black, gImage_red);
    /* END OF MH-ET LIVE initialization*/
    
    while(true){
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks();
        
        DelayMs(1000);
        voltage = LC709203F_Get_Cell_Voltage();
        
        /*
         * LED_TEST_1 RB8
         * LED_TEST_2 RB9
         * BTN_TST_1  RF5
         * BTN_TST_2  RF6
         * BUTTON 1 RE1
         * BUTTON 2 RF1
         * BUTTON 3 RF0
         
         */
       
        //I2C1_WriteRead((uint16_t)GAUGE_ADDRESS, &gauge_id_cmd[0], (size_t)wrcmd, &gauge_lecture[0], 3);

        printf("V=%d\r\n", voltage);
        voltage = LC709203F_Get_RSOC();
        printf("RSOC=%d\r\n", voltage);
     
        DelayMs(1000);
        //LC709203F_Set_Initial_RSOC();
       
       
        /*
        PMS_Passive_Read(pm1, pm25, pm10);
        printf("pm1.0 = %d\n", pm1);
        printf("pm2.5 = %d\n", pm25);
        printf("pm10 = %d\n", pm10);  
        printf("\n");
        PMS_Set_Sleep_Mode();   //Send PMS7003 to sleep
        
        SHT_Get_Temp_and_Humidity(tempd, humid);
        printf("Temperatura = %f\n", tempd);
        printf("Humedad = %f\n", humid);
        */
        
    }
    
    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

