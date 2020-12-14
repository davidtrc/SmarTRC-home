/*******************************************************************************
  System Interrupts File

  Company:
    Microchip Technology Inc.

  File Name:
    interrupt_a.S

  Summary:
    Raw ISR definitions.

  Description:
    This file maps all the interrupt vectors to their corresponding
    implementations. If a particular module interrupt is used, then its ISR
    definition can be found in corresponding PLIB source file. If a module
    interrupt is not used, then its ISR implementation is mapped to dummy
    handler.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <xc.h>
#include "ISR_Support.h"

    .extern  SPI1_RX_Handler

    .section   .vector_110,code, keep
    .equ     __vector_dispatch_110, IntVectorSPI1_RX_Handler
    .global  __vector_dispatch_110
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorSPI1_RX_Handler

IntVectorSPI1_RX_Handler:
    portSAVE_CONTEXT
    la    s6,  SPI1_RX_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorSPI1_RX_Handler
    .extern  SPI1_TX_Handler

    .section   .vector_111,code, keep
    .equ     __vector_dispatch_111, IntVectorSPI1_TX_Handler
    .global  __vector_dispatch_111
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorSPI1_TX_Handler

IntVectorSPI1_TX_Handler:
    portSAVE_CONTEXT
    la    s6,  SPI1_TX_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorSPI1_TX_Handler
    .extern  I2C1_BUS_Handler

    .section   .vector_115,code, keep
    .equ     __vector_dispatch_115, IntVectorI2C1_BUS_Handler
    .global  __vector_dispatch_115
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorI2C1_BUS_Handler

IntVectorI2C1_BUS_Handler:
    portSAVE_CONTEXT
    la    s6,  I2C1_BUS_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorI2C1_BUS_Handler
    .extern  I2C1_MASTER_Handler

    .section   .vector_117,code, keep
    .equ     __vector_dispatch_117, IntVectorI2C1_MASTER_Handler
    .global  __vector_dispatch_117
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorI2C1_MASTER_Handler

IntVectorI2C1_MASTER_Handler:
    portSAVE_CONTEXT
    la    s6,  I2C1_MASTER_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorI2C1_MASTER_Handler
    .extern  SPI2_RX_Handler

    .section   .vector_143,code, keep
    .equ     __vector_dispatch_143, IntVectorSPI2_RX_Handler
    .global  __vector_dispatch_143
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorSPI2_RX_Handler

IntVectorSPI2_RX_Handler:
    portSAVE_CONTEXT
    la    s6,  SPI2_RX_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorSPI2_RX_Handler
    .extern  SPI2_TX_Handler

    .section   .vector_144,code, keep
    .equ     __vector_dispatch_144, IntVectorSPI2_TX_Handler
    .global  __vector_dispatch_144
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorSPI2_TX_Handler

IntVectorSPI2_TX_Handler:
    portSAVE_CONTEXT
    la    s6,  SPI2_TX_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorSPI2_TX_Handler
    .extern  SPI3_RX_Handler

    .section   .vector_155,code, keep
    .equ     __vector_dispatch_155, IntVectorSPI3_RX_Handler
    .global  __vector_dispatch_155
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorSPI3_RX_Handler

IntVectorSPI3_RX_Handler:
    portSAVE_CONTEXT
    la    s6,  SPI3_RX_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorSPI3_RX_Handler
    .extern  SPI3_TX_Handler

    .section   .vector_156,code, keep
    .equ     __vector_dispatch_156, IntVectorSPI3_TX_Handler
    .global  __vector_dispatch_156
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorSPI3_TX_Handler

IntVectorSPI3_TX_Handler:
    portSAVE_CONTEXT
    la    s6,  SPI3_TX_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorSPI3_TX_Handler
    .extern  I2C3_BUS_Handler

    .section   .vector_160,code, keep
    .equ     __vector_dispatch_160, IntVectorI2C3_BUS_Handler
    .global  __vector_dispatch_160
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorI2C3_BUS_Handler

IntVectorI2C3_BUS_Handler:
    portSAVE_CONTEXT
    la    s6,  I2C3_BUS_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorI2C3_BUS_Handler
    .extern  I2C3_MASTER_Handler

    .section   .vector_162,code, keep
    .equ     __vector_dispatch_162, IntVectorI2C3_MASTER_Handler
    .global  __vector_dispatch_162
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorI2C3_MASTER_Handler

IntVectorI2C3_MASTER_Handler:
    portSAVE_CONTEXT
    la    s6,  I2C3_MASTER_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorI2C3_MASTER_Handler
    .extern  SPI4_RX_Handler

    .section   .vector_164,code, keep
    .equ     __vector_dispatch_164, IntVectorSPI4_RX_Handler
    .global  __vector_dispatch_164
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorSPI4_RX_Handler

IntVectorSPI4_RX_Handler:
    portSAVE_CONTEXT
    la    s6,  SPI4_RX_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorSPI4_RX_Handler
    .extern  SPI4_TX_Handler

    .section   .vector_165,code, keep
    .equ     __vector_dispatch_165, IntVectorSPI4_TX_Handler
    .global  __vector_dispatch_165
    .set     nomicromips
    .set     noreorder
    .set     nomips16
    .set     noat
    .ent  IntVectorSPI4_TX_Handler

IntVectorSPI4_TX_Handler:
    portSAVE_CONTEXT
    la    s6,  SPI4_TX_Handler
    jalr  s6
    nop
    portRESTORE_CONTEXT
    .end   IntVectorSPI4_TX_Handler

