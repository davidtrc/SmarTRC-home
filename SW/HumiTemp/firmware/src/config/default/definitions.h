/*******************************************************************************
  System Definitions

  File Name:
    definitions.h

  Summary:
    project system definitions.

  Description:
    This file contains the system-wide prototypes and definitions for a project.

 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
//DOM-IGNORE-END

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "peripheral/i2c/plib_i2c1.h"
#include "peripheral/uart/plib_uart3.h"
#include "peripheral/i2c/plib_i2c3.h"
#include "peripheral/uart/plib_uart2.h"
#include "peripheral/spi/plib_spi3.h"
#include "peripheral/clk/plib_clk.h"
#include "peripheral/gpio/plib_gpio.h"
#include "peripheral/evic/plib_evic.h"
#include "peripheral/tmr1/plib_tmr1.h"
#include "peripheral/spi/plib_spi2.h"
#include "peripheral/spi/plib_spi1.h"
#include "peripheral/nvm/plib_nvm.h"

/* Timer Counter Time period match values for input clock of 4096 Hz */
#define PMS_MEASURE_WAIT_MS                     35000
#define PERIOD_35S                              128*35
#define PERIOD_1MIN                             7680 
#define PERIOD_5MIN                             38400
#define PERIOD_10MIN                            76800
#define PERIOD_15MIN                            115200
#define PERIOD_20MIN                            153600
#define PERIOD_25MIN                            192000
#define PERIOD_30MIN                            230400
#define PERIOD_35MIN                            268800
#define PERIOD_40MIN                            307200
#define PERIOD_45MIN                            345600
#define PERIOD_50MIN                            384000
#define PERIOD_55MIN                            422400
#define PERIOD_60MIN                            460800

#define TIMER1_INT_GENERATED                    0x01
#define BTN1_INT_GENERATED                      0x02
#define BTN2_INT_GENERATED                      0x04
#define BTN3_INT_GENERATED                      0x08
#define ZIGBEE_INT_GENERATED                    0x10
#define SENSOR_INT_GENERATED                    0x20

#define USD_READY_TIMEOUT_MS                    500
#define USD_RX_TIMEOUT_MS                       100
#define USD_INIT_TIMEOUT_MS                     10000

volatile uint8_t isTmr1Expired;
volatile uint8_t btn1_interrupt;
volatile uint8_t btn2_interrupt;
volatile uint8_t btn3_interrupt;
volatile uint32_t btn1_interrupts_acc;
volatile uint32_t btn2_interrupts_acc;
volatile uint32_t btn3_interrupts_acc;
volatile uint8_t zigbee_interrupt;
volatile uint8_t sensor_interrupt;
volatile bool btn_test1_interrupt;
volatile bool btn_test2_interrupt;
volatile bool PMS_Data_Sent;
volatile bool PMS_Data_Received;

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: System Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* System Initialization Function

  Function:
    void SYS_Initialize( void *data )

  Summary:
    Function that initializes all modules in the system.

  Description:
    This function initializes all modules in the system, including any drivers,
    services, middleware, and applications.

  Precondition:
    None.

  Parameters:
    data            - Pointer to the data structure containing any data
                      necessary to initialize the module. This pointer may
                      be null if no data is required and default initialization
                      is to be used.

  Returns:
    None.

  Example:
    <code>
    SYS_Initialize ( NULL );

    while ( true )
    {
        SYS_Tasks ( );
    }
    </code>

  Remarks:
    This function will only be called once, after system reset.
*/

void SYS_Initialize( void *data );

/* Nullify SYS_Tasks() if only PLIBs are used. */
#define     SYS_Tasks()

// *****************************************************************************
// *****************************************************************************
// Section: extern declarations
// *****************************************************************************
// *****************************************************************************




//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* DEFINITIONS_H */
/*******************************************************************************
 End of File
*/

