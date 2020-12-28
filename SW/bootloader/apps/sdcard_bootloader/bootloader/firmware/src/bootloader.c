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
#include <stdio.h>
#include "peripheral/gpio/plib_gpio.h"
#include "peripheral/nvm/plib_nvm.h"
#include "bootloader/bootloader.h"
#include "sys/kmem.h"
#include "diskio.h"
#include "CustomTime.h"

#define APP_START_ADDRESS       ((uint32_t)(PA_TO_KVA0(0x1d000000UL)))
#define MAIN_APP_ADDRESS        (0x9D000000 + 0x1000)

#define FLASH_START             (0x9d000000UL)
#define FLASH_LENGTH            (0x200000UL)
#define PAGE_SIZE               (2048UL)
#define ERASE_BLOCK_SIZE        (16384UL)
#define PAGES_IN_ERASE_BLOCK    (ERASE_BLOCK_SIZE / PAGE_SIZE)

#define APP_IMAGE_FILE_NAME     "image.bin"

typedef enum
{

    BOOTLOADER_WAIT_FOR_DEVICE_ATTACH = 0,
            
    BOOTLOADER_INIT_USD,

    BOOTLOADER_VALIDATE_FILE_AND_OPEN,

    BOOTLOADER_ERASE_APPLICATION_SPACE,

    BOOTLOADER_READ_FILE,

    BOOTLOADER_PROCESS_FILE_BUFFER,

    BOOTLOADER_DEVICE_DETACHED,

    BOOTLOADER_ERROR,

} BOOTLOADER_STATES;


typedef struct
{
    /* Bootloader current state */
    BOOTLOADER_STATES currentState;

    /* Programming address */
    uint32_t progAddr;

    /* Stores Application binary file information */
    FIL fileStat;

    /* Flag to indicate device attached */
    bool deviceAttached;

} BOOTLOADER_DATA;

uint8_t CACHE_ALIGN fileBuffer[PAGE_SIZE];

BOOTLOADER_DATA btlData =
{
    .currentState   = BOOTLOADER_WAIT_FOR_DEVICE_ATTACH,
    .deviceAttached = false,
    .progAddr       = APP_START_ADDRESS
};

bool __WEAK bootloader_Trigger(void)
{
    /* Function can be overriden with custom implementation */
    return false;
}

static void bootloader_TriggerReset(void)
{
    /* Perform system unlock sequence */
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;

    RSWRSTSET = _RSWRST_SWRST_MASK;
    (void)RSWRST;
}

void run_Application(void)
{
    uint32_t msp            = *(uint32_t *)(MAIN_APP_ADDRESS);//(APP_START_ADDRESS);

    void (*fptr)(void);

    /* Set default to APP_RESET_ADDRESS */
    fptr = (void (*)(void))MAIN_APP_ADDRESS;//APP_START_ADDRESS;

    if (msp == 0xffffffff)
    {
        return;
    }
    
    __builtin_disable_interrupts();
    
    DelayMs(1000);

    fptr();
}

void bootloader_NvmAppErase( uint32_t appLength )
{
    uint32_t flashAddr = APP_START_ADDRESS;

    /* Align application binary length to erase boundary */
    appLength = appLength + (ERASE_BLOCK_SIZE - (appLength % ERASE_BLOCK_SIZE));

    while ((flashAddr < (FLASH_START + FLASH_LENGTH)) &&
           (appLength != 0))
    {
        NVM_PageErase(flashAddr);

        while(NVM_IsBusy() == true);

        flashAddr += ERASE_BLOCK_SIZE;
        appLength -= ERASE_BLOCK_SIZE;
    }
}

void bootloader_NVMPageWrite(uint8_t* data)
{
    NVM_RowWrite((uint32_t *)data, btlData.progAddr);

    while(NVM_IsBusy() == true);

    btlData.progAddr += PAGE_SIZE;
}

void bootloader_Tasks( void )
{
    
    
    UINT fileReadLength = 0;
    uint8_t uSD_switch = 0xFF;
    static FATFS Fatfs;
    static DSTATUS SD_stat;
    FRESULT op_file = 0XFF;
    
    uSD_switch = MICROSD_SW_Get();
    if(uSD_switch){                                                             //No card in the socket 
        btlData.deviceAttached = false;
        btlData.currentState = BOOTLOADER_DEVICE_DETACHED;
    } else {
       btlData.deviceAttached = true;
    }

    switch ( btlData.currentState )
    {

        case BOOTLOADER_WAIT_FOR_DEVICE_ATTACH:
        {
            if (btlData.deviceAttached == true)
            {
                printf("uSD detected\r\n");
                btlData.currentState = BOOTLOADER_INIT_USD;
            }
            break;
        }
        
        case BOOTLOADER_INIT_USD:
        {
            SD_stat = disk_initialize(0);
            if(SD_stat){
                printf("Card error = %d\r\n", SD_stat);
                break;
            }

            if(f_mount(&Fatfs, "", 0) == FR_INVALID_DRIVE){     //Mounting the file system
                printf("Invalid file system");
                break;
            }
            
            printf("uSD init\r\n");
            btlData.currentState = BOOTLOADER_VALIDATE_FILE_AND_OPEN;
            break;
        }

        case BOOTLOADER_VALIDATE_FILE_AND_OPEN:
        {
            
            op_file = f_open(&btlData.fileStat, APP_IMAGE_FILE_NAME, FA_READ);
            if(op_file){// Make sure the file is present and properly open
                break;
            } else {
                if(btlData.fileStat.obj.objsize<0){
                    break;
                }
                printf("APP file present and opened\r\n");
                btlData.currentState = BOOTLOADER_ERASE_APPLICATION_SPACE;      //File opened successfully. Erase Application space 
            }

            break;
        }

        case BOOTLOADER_ERASE_APPLICATION_SPACE:
        {
            bootloader_NvmAppErase(btlData.fileStat.obj.objsize);

            printf("Memory erased\r\n");
            btlData.currentState = BOOTLOADER_READ_FILE;

            memset((void *)fileBuffer, 0xFF, PAGE_SIZE);

            break;
        }

        case BOOTLOADER_READ_FILE:
        {
            f_read(&btlData.fileStat, (void *)fileBuffer, PAGE_SIZE, &fileReadLength);  //For a faster read, read 512 bytes at a time and buffer it.

            /* Reached End of File */
            if (fileReadLength <= 0)
            {
                f_close(&btlData.fileStat);
                printf("Updated completed. Reseting\r\n");
                
                bootloader_TriggerReset();
            }
            else
            {
                printf("Reading %d bytes of app file\r\n", fileReadLength);
                btlData.currentState = BOOTLOADER_PROCESS_FILE_BUFFER;
            }

            break;
        }

        case BOOTLOADER_PROCESS_FILE_BUFFER:
        {
            bootloader_NVMPageWrite(fileBuffer);

            memset((void *)fileBuffer, 0xFF, PAGE_SIZE);
            
            printf("Copying %lu bytes of app file\r\n", PAGE_SIZE);
            btlData.currentState = BOOTLOADER_READ_FILE;

            break;
        }

        case BOOTLOADER_DEVICE_DETACHED:
        {
            f_close(&btlData.fileStat);

            printf("Err. uSD detached. Update status: UNKNOWN. Reinstall with PicKit\r\n");
            btlData.currentState = BOOTLOADER_WAIT_FOR_DEVICE_ATTACH;
            break;
        }

        case BOOTLOADER_ERROR:
        default:
            break;
    }
}