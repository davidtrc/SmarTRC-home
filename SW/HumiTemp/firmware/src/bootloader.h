/*******************************************************************************
  Bootloader Header File

  File Name:
    bootloader.h

  Summary:
    This file contains Interface definitions of bootloder

  Description:
    This file defines interface for bootloader.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2019 Microchip Technology Inc. and its subsidiaries.
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
// DOM-IGNORE-END

#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdint.h>
#include <stdbool.h>

#include "sys/kmem.h"

#define BTL_TRIGGER_RAM_START               KVA0_TO_KVA1(0x80000000)
#define BTL_TRIGGER_PATTERN                 (0x5048434DUL)
#define BTL_CLEAR_TRIGGER_PATTERN           (0xFF00FF00UL)
#define APP_IMAGE_FILE_NAME                 "image.hex"

void bootloader_Clear_Trigger(void);
// *****************************************************************************
/* Function:
    bool bootloader_Trigger( void );

 Summary:
    Checks if Bootloader has to be executed at startup.

 Description:
    This function can be used to check for a External HW trigger or Internal firmware
    trigger to execute bootloader at startup.

    This check should happen before any system resources are initialized apart for PORT
    as the same system resource can be Re-initialized by the application if bootloader jumps
    to it and may cause issues.

    - <b>External Trigger: </b>
        Is achieved by triggering the selected GPIO_PIN in bootloader configuration
        in MHC.
    - <b>Firmware Trigger: </b>
        Application firmware which wants to execute bootloader at startup needs to
        fill first 16 bytes of ram location with bootloader request pattern.

        <code>
            uint32_t *sram = (uint32_t *)RAM_START_ADDRESS;

            sram[0] = 0x5048434D;
            sram[1] = 0x5048434D;
            sram[2] = 0x5048434D;
            sram[3] = 0x5048434D;
        </code>

 Precondition:
    PORT/PIO Initialize must have been called.

 Parameters:
    None.

 Returns:
    - True  : If any of trigger is detected.
    - False : If no trigger is detected..

 Example:
    <code>

        NVMCTRL_Initialize();

        PORT_Initialize();

        if (bootloader_Trigger() == false)
        {
            run_Application();
        }

        CLOCK_Initialize();

    </code>
*/
void bootloader_Trigger(void);

void bootloader_TriggerReset(void);

#endif