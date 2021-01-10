/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    bootloader.c

  Summary:
    Interface for the Bootloader library.

  Description:
    This file contains the interface definition for the Bootloader library.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2020 Microchip Technology Inc. and its subsidiaries.
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

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>

#include "peripheral/nvm/plib_nvm.h"
#include "bootloader.h"
#include "sys/kmem.h"

#include "definitions.h"                // SYS function prototypes
#include "CustomTime.h"

void bootloader_Clear_Trigger(void)
{
    /* Function can be overriden with custom implementation */
    
    uint32_t *sram = (uint32_t *)BTL_TRIGGER_RAM_START;

    sram[0] = BTL_CLEAR_TRIGGER_PATTERN;
    sram[1] = BTL_CLEAR_TRIGGER_PATTERN;
    sram[2] = BTL_CLEAR_TRIGGER_PATTERN;
    sram[3] = BTL_CLEAR_TRIGGER_PATTERN;
}

void bootloader_Trigger(void)
{
    /* Function can be overriden with custom implementation */
    
    uint32_t *sram = (uint32_t *)BTL_TRIGGER_RAM_START;

    sram[0] = BTL_TRIGGER_PATTERN;
    sram[1] = BTL_TRIGGER_PATTERN;
    sram[2] = BTL_TRIGGER_PATTERN;
    sram[3] = BTL_TRIGGER_PATTERN;
}

void bootloader_TriggerReset(void)
{
    __builtin_disable_interrupts();
    
    DelayMs(1000);
    
    /* Perform system unlock sequence */
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;

    RSWRSTSET = _RSWRST_SWRST_MASK;
    (void)RSWRST;
}