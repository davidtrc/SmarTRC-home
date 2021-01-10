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

#define BTL_TRIGGER_PATTERN (0x5048434DUL)

#define BTL_TRIGGER_RAM_START   KVA0_TO_KVA1(0x80000000)
#define BTL_CLEAR_TRIGGER_PATTERN (0x00FF00FFUL)

#define BTL_TRIGGER_LEN         16

#define DATA_RECORD 		0
#define END_OF_FILE_RECORD 	1
#define EXT_SEG_ADRS_RECORD 2
#define EXT_LIN_ADRS_RECORD 4

#define REC_FLASHED 0
#define REC_NOT_FOUND 1
#define REC_FOUND_BUT_NOT_FLASHED 2


typedef unsigned char		BYTE;				// 8-bit unsigned
typedef unsigned short int	WORD;				// 16-bit unsigned
typedef unsigned long		DWORD;				// 32-bit unsigned
typedef unsigned long long	QWORD;				// 64-bit unsigned
typedef signed char			CHAR;				// 8-bit signed
typedef signed short int	SHORT;				// 16-bit signed
typedef signed long			LONG;				// 32-bit signed
typedef signed long long	LONGLONG;			// 64-bit signed

/* Alternate definitions */
typedef void                VOID;

typedef char                CHAR8;
typedef unsigned char       UCHAR8;

/* Processor & Compiler independent, size specific definitions */
// To Do:  We need to verify the sizes on each compiler.  These
//         may be compiler specific, we should either move them
//         to "compiler.h" or #ifdef them for compiler type.
typedef signed int          INT;
typedef signed char         INT8;
typedef signed short int    INT16;
typedef signed long int     INT32;
typedef signed long long    INT64;

typedef unsigned int        UINT;
typedef unsigned char       UINT8;
typedef unsigned short int  UINT16;
typedef unsigned long int   UINT32;  // other name for 32-bit integer
typedef unsigned long long  UINT64;

typedef union _WORD_VAL
{
    WORD Val;
    BYTE v[2];
    struct
    {
        BYTE LB;
        BYTE HB;
    } byte;
    struct
    {
        unsigned char b0:1;
        unsigned char b1:1;
        unsigned char b2:1;
        unsigned char b3:1;
        unsigned char b4:1;
        unsigned char b5:1;
        unsigned char b6:1;
        unsigned char b7:1;
        unsigned char b8:1;
        unsigned char b9:1;
        unsigned char b10:1;
        unsigned char b11:1;
        unsigned char b12:1;
        unsigned char b13:1;
        unsigned char b14:1;
        unsigned char b15:1;
    } bits;
} WORD_VAL, WORD_BITS;

typedef union _DWORD_VAL
{
    DWORD Val;
	WORD w[2];
    BYTE v[4];
    struct
    {
        WORD LW;
        WORD HW;
    } word;
    struct
    {
        BYTE LB;
        BYTE HB;
        BYTE UB;
        BYTE MB;
    } byte;
    struct
    {
        WORD_VAL low;
        WORD_VAL high;
    }wordUnion;
    struct
    {
        unsigned char b0:1;
        unsigned char b1:1;
        unsigned char b2:1;
        unsigned char b3:1;
        unsigned char b4:1;
        unsigned char b5:1;
        unsigned char b6:1;
        unsigned char b7:1;
        unsigned char b8:1;
        unsigned char b9:1;
        unsigned char b10:1;
        unsigned char b11:1;
        unsigned char b12:1;
        unsigned char b13:1;
        unsigned char b14:1;
        unsigned char b15:1;
        unsigned char b16:1;
        unsigned char b17:1;
        unsigned char b18:1;
        unsigned char b19:1;
        unsigned char b20:1;
        unsigned char b21:1;
        unsigned char b22:1;
        unsigned char b23:1;
        unsigned char b24:1;
        unsigned char b25:1;
        unsigned char b26:1;
        unsigned char b27:1;
        unsigned char b28:1;
        unsigned char b29:1;
        unsigned char b30:1;
        unsigned char b31:1;
    } bits;
} DWORD_VAL;

typedef struct
{
    uint8_t *start;
    uint8_t len;
    uint8_t status;
}T_REC;

typedef struct 
{
	uint8_t RecDataLen;
	DWORD_VAL Address;
	uint8_t RecType;
	uint8_t* Data;
	uint8_t CheckSum;	
	DWORD_VAL ExtSegAddress;
	DWORD_VAL ExtLinAddress;
}T_HEX_RECORD;	

bool bootloader_Check_If_File_Exists(char *filename);

bool bootloader_ClearTrigger(void);

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
bool bootloader_Trigger( void );

// *****************************************************************************
/* Function:
    void run_Application( void );

 Summary:
    Runs the programmed application at startup.

 Description:
    This function can be used to run programmed application though bootloader at startup.

    If the first 4Bytes of Application Memory is not 0xFFFFFFFF then it jumps to
    the application start address to run the application programmed through bootloader and
    never returns.

    If the first 4Bytes of Application Memory is 0xFFFFFFFF then it returns from function
    and executes bootloader for accepting a new application firmware.

 Precondition:
    bootloader_Trigger() must be called to check for bootloader triggers at startup.

 Parameters:
    None.

 Returns:
    None

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
void run_Application( void );

// *****************************************************************************
/* Function:
    void bootloader_Tasks( void );

 Summary:
    Starts bootloader execution.

 Description:
    This function can be used to start bootloader execution.

    The function continuously waits for application firmware from the HOST via
    selected communication protocol to program into internal flash memory.

    Once the complete application is received, programmed and verified successfully,
    It resets the device to jump into programmed application.

    Note:
    For Optimized Bootloaders:
        - This function never returns.
        - This function will be directly called from main function

    For Unified and File System based Bootloaders:
        - This function returns once the state machine execution is completed
        - This function will be called from SYS_Tasks() routine from the super loop

 Precondition:
    bootloader_Trigger() must be called to check for bootloader triggers at startup.

 Parameters:
    None.

 Returns:
    None

 Example:
    <code>

        bootloader_Tasks();

    </code>
*/
void bootloader_Tasks( void );

#endif