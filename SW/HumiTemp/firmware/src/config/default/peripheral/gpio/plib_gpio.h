/*******************************************************************************
  GPIO PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_gpio.h

  Summary:
    GPIO PLIB Header File

  Description:
    This library provides an interface to control and interact with Parallel
    Input/Output controller (GPIO) module.

*******************************************************************************/

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

#ifndef PLIB_GPIO_H
#define PLIB_GPIO_H

#include <device.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data types and constants
// *****************************************************************************
// *****************************************************************************

/*** Macros for MHET_DC pin ***/
#define MHET_DC_Set()               (LATESET = (1<<6))
#define MHET_DC_Clear()             (LATECLR = (1<<6))
#define MHET_DC_Toggle()            (LATEINV= (1<<6))
#define MHET_DC_Get()               ((PORTE >> 6) & 0x1)
#define MHET_DC_OutputEnable()      (TRISECLR = (1<<6))
#define MHET_DC_InputEnable()       (TRISESET = (1<<6))
#define MHET_DC_PIN                  GPIO_PIN_RE6
/*** Macros for MHET_RST pin ***/
#define MHET_RST_Set()               (LATESET = (1<<7))
#define MHET_RST_Clear()             (LATECLR = (1<<7))
#define MHET_RST_Toggle()            (LATEINV= (1<<7))
#define MHET_RST_Get()               ((PORTE >> 7) & 0x1)
#define MHET_RST_OutputEnable()      (TRISECLR = (1<<7))
#define MHET_RST_InputEnable()       (TRISESET = (1<<7))
#define MHET_RST_PIN                  GPIO_PIN_RE7
/*** Macros for MHET_CS pin ***/
#define MHET_CS_Set()               (LATGSET = (1<<9))
#define MHET_CS_Clear()             (LATGCLR = (1<<9))
#define MHET_CS_Toggle()            (LATGINV= (1<<9))
#define MHET_CS_Get()               ((PORTG >> 9) & 0x1)
#define MHET_CS_OutputEnable()      (TRISGCLR = (1<<9))
#define MHET_CS_InputEnable()       (TRISGSET = (1<<9))
#define MHET_CS_PIN                  GPIO_PIN_RG9
/*** Macros for MICROSD_SW pin ***/
#define MICROSD_SW_Set()               (LATBSET = (1<<4))
#define MICROSD_SW_Clear()             (LATBCLR = (1<<4))
#define MICROSD_SW_Toggle()            (LATBINV= (1<<4))
#define MICROSD_SW_Get()               ((PORTB >> 4) & 0x1)
#define MICROSD_SW_OutputEnable()      (TRISBCLR = (1<<4))
#define MICROSD_SW_InputEnable()       (TRISBSET = (1<<4))
#define MICROSD_SW_PIN                  GPIO_PIN_RB4
/*** Macros for LED_TEST_1 pin ***/
#define LED_TEST_1_Set()               (LATBSET = (1<<8))
#define LED_TEST_1_Clear()             (LATBCLR = (1<<8))
#define LED_TEST_1_Toggle()            (LATBINV= (1<<8))
#define LED_TEST_1_Get()               ((PORTB >> 8) & 0x1)
#define LED_TEST_1_OutputEnable()      (TRISBCLR = (1<<8))
#define LED_TEST_1_InputEnable()       (TRISBSET = (1<<8))
#define LED_TEST_1_PIN                  GPIO_PIN_RB8
/*** Macros for LED_TEST_2 pin ***/
#define LED_TEST_2_Set()               (LATBSET = (1<<9))
#define LED_TEST_2_Clear()             (LATBCLR = (1<<9))
#define LED_TEST_2_Toggle()            (LATBINV= (1<<9))
#define LED_TEST_2_Get()               ((PORTB >> 9) & 0x1)
#define LED_TEST_2_OutputEnable()      (TRISBCLR = (1<<9))
#define LED_TEST_2_InputEnable()       (TRISBSET = (1<<9))
#define LED_TEST_2_PIN                  GPIO_PIN_RB9
/*** Macros for POWER_5V_EN pin ***/
#define POWER_5V_EN_Set()               (LATBSET = (1<<11))
#define POWER_5V_EN_Clear()             (LATBCLR = (1<<11))
#define POWER_5V_EN_Toggle()            (LATBINV= (1<<11))
#define POWER_5V_EN_Get()               ((PORTB >> 11) & 0x1)
#define POWER_5V_EN_OutputEnable()      (TRISBCLR = (1<<11))
#define POWER_5V_EN_InputEnable()       (TRISBSET = (1<<11))
#define POWER_5V_EN_PIN                  GPIO_PIN_RB11
/*** Macros for PMS_EN2 pin ***/
#define PMS_EN2_Set()               (LATBSET = (1<<12))
#define PMS_EN2_Clear()             (LATBCLR = (1<<12))
#define PMS_EN2_Toggle()            (LATBINV= (1<<12))
#define PMS_EN2_Get()               ((PORTB >> 12) & 0x1)
#define PMS_EN2_OutputEnable()      (TRISBCLR = (1<<12))
#define PMS_EN2_InputEnable()       (TRISBSET = (1<<12))
#define PMS_EN2_PIN                  GPIO_PIN_RB12
/*** Macros for PMS_EN pin ***/
#define PMS_EN_Set()               (LATCSET = (1<<12))
#define PMS_EN_Clear()             (LATCCLR = (1<<12))
#define PMS_EN_Toggle()            (LATCINV= (1<<12))
#define PMS_EN_Get()               ((PORTC >> 12) & 0x1)
#define PMS_EN_OutputEnable()      (TRISCCLR = (1<<12))
#define PMS_EN_InputEnable()       (TRISCSET = (1<<12))
#define PMS_EN_PIN                  GPIO_PIN_RC12
/*** Macros for PMS_RST pin ***/
#define PMS_RST_Set()               (LATCSET = (1<<15))
#define PMS_RST_Clear()             (LATCCLR = (1<<15))
#define PMS_RST_Toggle()            (LATCINV= (1<<15))
#define PMS_RST_Get()               ((PORTC >> 15) & 0x1)
#define PMS_RST_OutputEnable()      (TRISCCLR = (1<<15))
#define PMS_RST_InputEnable()       (TRISCSET = (1<<15))
#define PMS_RST_PIN                  GPIO_PIN_RC15
/*** Macros for BTN_TST_2 pin ***/
#define BTN_TST_2_Set()               (LATFSET = (1<<4))
#define BTN_TST_2_Clear()             (LATFCLR = (1<<4))
#define BTN_TST_2_Toggle()            (LATFINV= (1<<4))
#define BTN_TST_2_Get()               ((PORTF >> 4) & 0x1)
#define BTN_TST_2_OutputEnable()      (TRISFCLR = (1<<4))
#define BTN_TST_2_InputEnable()       (TRISFSET = (1<<4))
#define BTN_TST_2_InterruptEnable()   (CNENFSET = (1<<4))
#define BTN_TST_2_InterruptDisable()  (CNENFCLR = (1<<4))
#define BTN_TST_2_PIN                  GPIO_PIN_RF4
/*** Macros for BTN_TST_1 pin ***/
#define BTN_TST_1_Set()               (LATFSET = (1<<5))
#define BTN_TST_1_Clear()             (LATFCLR = (1<<5))
#define BTN_TST_1_Toggle()            (LATFINV= (1<<5))
#define BTN_TST_1_Get()               ((PORTF >> 5) & 0x1)
#define BTN_TST_1_OutputEnable()      (TRISFCLR = (1<<5))
#define BTN_TST_1_InputEnable()       (TRISFSET = (1<<5))
#define BTN_TST_1_InterruptEnable()   (CNENFSET = (1<<5))
#define BTN_TST_1_InterruptDisable()  (CNENFCLR = (1<<5))
#define BTN_TST_1_PIN                  GPIO_PIN_RF5
/*** Macros for ZIGBEE_WAKE pin ***/
#define ZIGBEE_WAKE_Set()               (LATESET = (1<<0))
#define ZIGBEE_WAKE_Clear()             (LATECLR = (1<<0))
#define ZIGBEE_WAKE_Toggle()            (LATEINV= (1<<0))
#define ZIGBEE_WAKE_Get()               ((PORTE >> 0) & 0x1)
#define ZIGBEE_WAKE_OutputEnable()      (TRISECLR = (1<<0))
#define ZIGBEE_WAKE_InputEnable()       (TRISESET = (1<<0))
#define ZIGBEE_WAKE_PIN                  GPIO_PIN_RE0
/*** Macros for ZIGBEE_RST pin ***/
#define ZIGBEE_RST_Set()               (LATESET = (1<<1))
#define ZIGBEE_RST_Clear()             (LATECLR = (1<<1))
#define ZIGBEE_RST_Toggle()            (LATEINV= (1<<1))
#define ZIGBEE_RST_Get()               ((PORTE >> 1) & 0x1)
#define ZIGBEE_RST_OutputEnable()      (TRISECLR = (1<<1))
#define ZIGBEE_RST_InputEnable()       (TRISESET = (1<<1))
#define ZIGBEE_RST_PIN                  GPIO_PIN_RE1
/*** Macros for SENSOR_RST pin ***/
#define SENSOR_RST_Set()               (LATESET = (1<<2))
#define SENSOR_RST_Clear()             (LATECLR = (1<<2))
#define SENSOR_RST_Toggle()            (LATEINV= (1<<2))
#define SENSOR_RST_Get()               ((PORTE >> 2) & 0x1)
#define SENSOR_RST_OutputEnable()      (TRISECLR = (1<<2))
#define SENSOR_RST_InputEnable()       (TRISESET = (1<<2))
#define SENSOR_RST_PIN                  GPIO_PIN_RE2
/*** Macros for MHET_BUSY pin ***/
#define MHET_BUSY_Set()               (LATESET = (1<<4))
#define MHET_BUSY_Clear()             (LATECLR = (1<<4))
#define MHET_BUSY_Toggle()            (LATEINV= (1<<4))
#define MHET_BUSY_Get()               ((PORTE >> 4) & 0x1)
#define MHET_BUSY_OutputEnable()      (TRISECLR = (1<<4))
#define MHET_BUSY_InputEnable()       (TRISESET = (1<<4))
#define MHET_BUSY_PIN                  GPIO_PIN_RE4


// *****************************************************************************
/* GPIO Port

  Summary:
    Identifies the available GPIO Ports.

  Description:
    This enumeration identifies the available GPIO Ports.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all ports are available on all devices.  Refer to the specific
    device data sheet to determine which ports are supported.
*/

typedef enum
{
    GPIO_PORT_B = 0,
    GPIO_PORT_C = 1,
    GPIO_PORT_D = 2,
    GPIO_PORT_E = 3,
    GPIO_PORT_F = 4,
    GPIO_PORT_G = 5,
} GPIO_PORT;

// *****************************************************************************
/* GPIO Port Pins

  Summary:
    Identifies the available GPIO port pins.

  Description:
    This enumeration identifies the available GPIO port pins.

  Remarks:
    The caller should not rely on the specific numbers assigned to any of
    these values as they may change from one processor to the next.

    Not all pins are available on all devices.  Refer to the specific
    device data sheet to determine which pins are supported.
*/

typedef enum
{
    GPIO_PIN_RB0 = 0,
    GPIO_PIN_RB1 = 1,
    GPIO_PIN_RB2 = 2,
    GPIO_PIN_RB3 = 3,
    GPIO_PIN_RB4 = 4,
    GPIO_PIN_RB5 = 5,
    GPIO_PIN_RB6 = 6,
    GPIO_PIN_RB7 = 7,
    GPIO_PIN_RB8 = 8,
    GPIO_PIN_RB9 = 9,
    GPIO_PIN_RB10 = 10,
    GPIO_PIN_RB11 = 11,
    GPIO_PIN_RB12 = 12,
    GPIO_PIN_RB13 = 13,
    GPIO_PIN_RB14 = 14,
    GPIO_PIN_RB15 = 15,
    GPIO_PIN_RC12 = 28,
    GPIO_PIN_RC13 = 29,
    GPIO_PIN_RC14 = 30,
    GPIO_PIN_RC15 = 31,
    GPIO_PIN_RD0 = 32,
    GPIO_PIN_RD1 = 33,
    GPIO_PIN_RD2 = 34,
    GPIO_PIN_RD3 = 35,
    GPIO_PIN_RD4 = 36,
    GPIO_PIN_RD5 = 37,
    GPIO_PIN_RD9 = 41,
    GPIO_PIN_RD10 = 42,
    GPIO_PIN_RD11 = 43,
    GPIO_PIN_RE0 = 48,
    GPIO_PIN_RE1 = 49,
    GPIO_PIN_RE2 = 50,
    GPIO_PIN_RE3 = 51,
    GPIO_PIN_RE4 = 52,
    GPIO_PIN_RE5 = 53,
    GPIO_PIN_RE6 = 54,
    GPIO_PIN_RE7 = 55,
    GPIO_PIN_RF0 = 64,
    GPIO_PIN_RF1 = 65,
    GPIO_PIN_RF3 = 67,
    GPIO_PIN_RF4 = 68,
    GPIO_PIN_RF5 = 69,
    GPIO_PIN_RG6 = 86,
    GPIO_PIN_RG7 = 87,
    GPIO_PIN_RG8 = 88,
    GPIO_PIN_RG9 = 89,

    /* This element should not be used in any of the GPIO APIs.
       It will be used by other modules or application to denote that none of the GPIO Pin is used */
    GPIO_PIN_NONE = -1

} GPIO_PIN;

typedef  void (*GPIO_PIN_CALLBACK) ( GPIO_PIN pin, uintptr_t context);

void GPIO_Initialize(void);

// *****************************************************************************
// *****************************************************************************
// Section: GPIO Functions which operates on multiple pins of a port
// *****************************************************************************
// *****************************************************************************

uint32_t GPIO_PortRead(GPIO_PORT port);

void GPIO_PortWrite(GPIO_PORT port, uint32_t mask, uint32_t value);

uint32_t GPIO_PortLatchRead ( GPIO_PORT port );

void GPIO_PortSet(GPIO_PORT port, uint32_t mask);

void GPIO_PortClear(GPIO_PORT port, uint32_t mask);

void GPIO_PortToggle(GPIO_PORT port, uint32_t mask);

void GPIO_PortInputEnable(GPIO_PORT port, uint32_t mask);

void GPIO_PortOutputEnable(GPIO_PORT port, uint32_t mask);

void GPIO_PortInterruptEnable(GPIO_PORT port, uint32_t mask);

void GPIO_PortInterruptDisable(GPIO_PORT port, uint32_t mask);

// *****************************************************************************
// *****************************************************************************
// Section: Local Data types and Prototypes
// *****************************************************************************
// *****************************************************************************

typedef struct {

    /* target pin */
    GPIO_PIN                 pin;

    /* Callback for event on target pin*/
    GPIO_PIN_CALLBACK        callback;

    /* Callback Context */
    uintptr_t               context;

} GPIO_PIN_CALLBACK_OBJ;

// *****************************************************************************
// *****************************************************************************
// Section: GPIO Functions which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

static inline void GPIO_PinWrite(GPIO_PIN pin, bool value)
{
    GPIO_PortWrite(pin>>4, (uint32_t)(0x1) << (pin & 0xF), (uint32_t)(value) << (pin & 0xF));
}

static inline bool GPIO_PinRead(GPIO_PIN pin)
{
    return (bool)(((GPIO_PortRead(pin>>4)) >> (pin & 0xF)) & 0x1);
}

static inline bool GPIO_PinLatchRead(GPIO_PIN pin)
{
    return (bool)((GPIO_PortLatchRead(pin>>4) >> (pin & 0xF)) & 0x1);
}

static inline void GPIO_PinToggle(GPIO_PIN pin)
{
    GPIO_PortToggle(pin>>4, 0x1 << (pin & 0xF));
}

static inline void GPIO_PinSet(GPIO_PIN pin)
{
    GPIO_PortSet(pin>>4, 0x1 << (pin & 0xF));
}

static inline void GPIO_PinClear(GPIO_PIN pin)
{
    GPIO_PortClear(pin>>4, 0x1 << (pin & 0xF));
}

static inline void GPIO_PinInputEnable(GPIO_PIN pin)
{
    GPIO_PortInputEnable(pin>>4, 0x1 << (pin & 0xF));
}

static inline void GPIO_PinOutputEnable(GPIO_PIN pin)
{
    GPIO_PortOutputEnable(pin>>4, 0x1 << (pin & 0xF));
}

static inline void GPIO_PinInterruptEnable(GPIO_PIN pin)
{
    GPIO_PortInterruptEnable(pin>>4, 0x1 << (pin & 0xF));
}

static inline void GPIO_PinInterruptDisable(GPIO_PIN pin)
{
    GPIO_PortInterruptDisable(pin>>4, 0x1 << (pin & 0xF));
}

bool GPIO_PinInterruptCallbackRegister(
    GPIO_PIN pin,
    const   GPIO_PIN_CALLBACK callBack,
    uintptr_t context
);

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif
// DOM-IGNORE-END
#endif // PLIB_GPIO_H
