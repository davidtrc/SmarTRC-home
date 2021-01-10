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
#include "user.h"
#include "toolchain_specifics.h"

#include "peripheral/gpio/plib_gpio.h"
#include "peripheral/nvm/plib_nvm.h"
#include "bootloader/bootloader.h"
#include "sys/kmem.h"
#include "diskio.h"
#include "CustomTime.h"


#define REBOOT_ADDRESS        (0x9D000000 + 0x1000)

#define USER_VARIABLES_ADDRESS  (0x9D000000 + 0x4000)

#define MAIN_APP_ADDRESS        (0x9D000000 + 0x8000)

#define FLASH_START             (0x9d000000U)
#define FLASH_LENGTH            (0x200000U)

#define PAGE_SIZE               (16384U)
#define ERASE_BLOCK_SIZE        (16384U)

#define APP_IMAGE_FILE_NAME     "image.hex"

#define CHECKSUM_ERROR          0x01

typedef enum
{
    BOOTLOADER_WAIT_FOR_DEVICE_ATTACH = 0,
            
    BOOTLOADER_INIT_USD,
            
    BOOTLOADER_INIT_USD_AFTER_FAIL,

    BOOTLOADER_VALIDATE_FILE_AND_OPEN,

    BOOTLOADER_ERASE_APPLICATION_SPACE,

    BOOTLOADER_READ_FILE,
            
    BOOTLOADER_VALIDATE_FILE_AND_OPEN_AFTER_FAIL,

    BOOTLOADER_PROCESS_FILE_BUFFER,

    BOOTLOADER_DEVICE_DETACHED,

    BOOTLOADER_ERROR,
            
    BOOTLOADER_CHECKSUM_ERROR,

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

//uint8_t CACHE_ALIGN fileBuffer[PAGE_SIZE];
UINT8 fileBuffer[1024];
UINT pointer = 0;
UINT8 hexRec[100];
T_REC record;
volatile double file_read_percentage = 0;

BOOTLOADER_DATA btlData =
{
    .currentState   = BOOTLOADER_WAIT_FOR_DEVICE_ATTACH,
    .deviceAttached = false,
    .progAddr       = MAIN_APP_ADDRESS
};

void ConvertAsciiToHex(uint8_t* asciiRec, uint8_t* hexRec);
uint8_t WriteHexRecord2Flash(uint8_t* HexRecord);

bool bootloader_ClearTrigger(void)
{
    /* Function can be overriden with custom implementation */
    
    uint32_t *sram = (uint32_t *)BTL_TRIGGER_RAM_START;

    sram[0] = BTL_CLEAR_TRIGGER_PATTERN;
    sram[1] = BTL_CLEAR_TRIGGER_PATTERN;
    sram[2] = BTL_CLEAR_TRIGGER_PATTERN;
    sram[3] = BTL_CLEAR_TRIGGER_PATTERN;
    
    return false;
}

bool bootloader_Trigger(void)
{
    uint32_t i;
    static uint32_t *ramStart = (uint32_t *)BTL_TRIGGER_RAM_START;

    // Cheap delay. This should give at leat 1 ms delay.
    for (i = 0; i < 2000; i++)
    {
        asm("nop");
    }

    /* Check for Bootloader Trigger Pattern in first 16 Bytes of RAM to enter
     * Bootloader.
     */
    if (BTL_TRIGGER_PATTERN == ramStart[0] && BTL_TRIGGER_PATTERN == ramStart[1] &&
        BTL_TRIGGER_PATTERN == ramStart[2] && BTL_TRIGGER_PATTERN == ramStart[3])
    {
        
        printf("RAM condition MET\r\n");

        DCACHE_CLEAN_BY_ADDR(ramStart, 4);
        DelayMs(1000);
        return true;
    }

    return false;
}

void run_Application(void)
{
    uint32_t msp            = *(uint32_t *)(REBOOT_ADDRESS);

    void (*fptr)(void);

    /* Set default to APP_RESET_ADDRESS */
    fptr = (void (*)(void))REBOOT_ADDRESS;

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
    uint32_t flashAddr = FLASH_START; //MAIN_APP_ADDRESS con esto llego a instalarse bien

    /* Align application binary length to erase boundary */
    appLength = appLength + (ERASE_BLOCK_SIZE - (appLength % ERASE_BLOCK_SIZE));

    while ((flashAddr < (FLASH_START + FLASH_LENGTH)) && (appLength != 0)) //Erase APP zone BUT KEEPING NVM for user variables (located in 9D004000-9D008000)
    {
        if(flashAddr != USER_VARIABLES_ADDRESS){
            NVM_PageErase(flashAddr);
            printf("Erasing data at %X\n", flashAddr);

            while(NVM_IsBusy() == true);
        }

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
    #define BYTES_READ_PER_PERIOD       512

    static UINT fileReadLength = 0;
    uint8_t uSD_switch = 0xFF;
    static FATFS Fatfs;
    static DSTATUS SD_stat;
    FRESULT op_file = 0XFF;
    static volatile uint32_t rd_counter = 0;
    uint8_t flashwrreturn = 0;
    FRESULT rd_file_st = 0xFF;
    
    volatile UINT i;
    
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
                DelayMs(500);
                btlData.currentState = BOOTLOADER_INIT_USD;
            }
            break;
        }
        
        case BOOTLOADER_INIT_USD:
        {
            SD_stat = disk_initialize(0);
            if(SD_stat){
                printf("Card error = %d\r\n", SD_stat);
                DelayMs(500);
                break;
            }

            if(f_mount(&Fatfs, "", 0) == FR_INVALID_DRIVE){     //Mounting the file system
                printf("Invalid file system\r\n");
                DelayMs(500);
                break;
            }
            
            printf("uSD init\r\n");
            DelayMs(500);
            btlData.currentState = BOOTLOADER_VALIDATE_FILE_AND_OPEN;
            break;
        }

        case BOOTLOADER_VALIDATE_FILE_AND_OPEN:
        {  
            op_file = f_open(&btlData.fileStat, APP_IMAGE_FILE_NAME, FA_READ);
            if(op_file){// Make sure the file is present and properly open
                printf("APP file not present\r\n");
                break;
            } else {
                if(btlData.fileStat.obj.objsize<0){
                    break;
                }
                unsigned long filesize = (unsigned long) btlData.fileStat.obj.objsize;
                printf("APP file present and opened. Size = %lu bytes\r\n", filesize);
                btlData.currentState = BOOTLOADER_ERASE_APPLICATION_SPACE;      //File opened successfully. Erase Application space 
            }

            break;
        }

        case BOOTLOADER_ERASE_APPLICATION_SPACE:
        {
            bootloader_NvmAppErase(btlData.fileStat.obj.objsize);

            printf("Memory erased\r\n");
            btlData.currentState = BOOTLOADER_READ_FILE;

            memset((void *)fileBuffer, 0xFF, sizeof(fileBuffer));
            record.status = REC_NOT_FOUND;

            break;
        }

        case BOOTLOADER_READ_FILE:
        {
            // For a faster read, read 512 bytes at a time and buffer it.
            rd_file_st = f_read(&btlData.fileStat, (void *)&fileBuffer[pointer], 512, &fileReadLength);  //For a faster read, read 512 bytes at a time and buffer it.
            if(fileReadLength == 512){
                file_read_percentage = ((fileReadLength + (rd_counter*512.00)) / btlData.fileStat.obj.objsize ) * 100.00;
                //printf("RD at %d\n", rd_counter*512);
                rd_counter++; 
            }
            DelayMs(150);
            if ( (fileReadLength == 0) || (rd_file_st != FR_OK) )
            {
                if(rd_file_st != FR_OK){
                    btlData.currentState = BOOTLOADER_INIT_USD_AFTER_FAIL;
                    break;
                }
            }
            
            btlData.currentState = BOOTLOADER_PROCESS_FILE_BUFFER;
            break;         
        }

        case BOOTLOADER_PROCESS_FILE_BUFFER:
        {
            for(i = 0; i < (fileReadLength + pointer); i++)
            {

             // This state machine seperates-out the valid hex records from the read 512 bytes.
                switch(record.status)
                {
                    case REC_FLASHED:
                    case REC_NOT_FOUND:
                        if(fileBuffer[i] == ':')
                        {
                         // We have a record found in the 512 bytes of data in the buffer.
                            record.start = &fileBuffer[i];
                            record.len = 0;
                            record.status = REC_FOUND_BUT_NOT_FLASHED;
                        }
                        break;
                    case REC_FOUND_BUT_NOT_FLASHED:
                        if((fileBuffer[i] == 0x0A) || (fileBuffer[i] == 0xFF))
                        {
                         // We have got a complete record. (0x0A is new line feed and 0xFF is End of file)
                            // Start the hex conversion from element
                            // 1. This will discard the ':' which is
                            // the start of the hex record.
                            ConvertAsciiToHex(&record.start[1], hexRec);
                            flashwrreturn = WriteHexRecord2Flash(hexRec);
                            if(flashwrreturn == CHECKSUM_ERROR){
                                memset(hexRec, 0, sizeof(hexRec));
                                memset(fileBuffer, 0, sizeof(fileBuffer));
                                pointer = 0;
                                printf("Checksum err at RD %d .P=%f\r\n", (rd_counter-1)*512, file_read_percentage);
                                record.status = REC_NOT_FOUND;
                                btlData.currentState = BOOTLOADER_INIT_USD_AFTER_FAIL;
                                break;
                            }
                            record.status = REC_FLASHED;
                        }
                        break;
                }
                if(flashwrreturn == CHECKSUM_ERROR){
                    break;
                }
                // Move to next byte in the buffer.
                record.len++;
            }
            
            if(flashwrreturn == CHECKSUM_ERROR){
                break;
            }

            if(record.status == REC_FOUND_BUT_NOT_FLASHED)
            {
             // We still have a half read record in the buffer. The next half part of the record is read when we read 512 bytes of data from the next file read. 
                memcpy(fileBuffer, record.start, record.len);
                pointer = record.len;
                record.status = REC_NOT_FOUND;
            }
            else
            {
                pointer = 0;
            }
            
            btlData.currentState = BOOTLOADER_READ_FILE;

            break;
        }
        
        case BOOTLOADER_CHECKSUM_ERROR:
        {
            rd_counter--;
            printf("Rewinding file to %d\r\n", rd_counter*512);
            
            f_lseek(&btlData.fileStat, (rd_counter*512));
            
            
            btlData.currentState = BOOTLOADER_READ_FILE;

            break;
        }
        
        case BOOTLOADER_INIT_USD_AFTER_FAIL:
        {
            SD_stat = disk_initialize(0);
            if(SD_stat){
                printf("Card error = %d\r\n", SD_stat);
                DelayMs(500);
                break;
            }

            if(f_mount(&Fatfs, "", 0) == FR_INVALID_DRIVE){     //Mounting the file system
                printf("Invalid file system\r\n");
                DelayMs(500);
                break;
            }
            
            printf("uSD init\r\n");
            DelayMs(500);
            btlData.currentState = BOOTLOADER_VALIDATE_FILE_AND_OPEN_AFTER_FAIL;
            break;
        }
        
        case BOOTLOADER_VALIDATE_FILE_AND_OPEN_AFTER_FAIL:
        {
            op_file = f_open(&btlData.fileStat, APP_IMAGE_FILE_NAME, FA_READ);
            if(op_file){// Make sure the file is present and properly open
                printf("APP file not present\r\n");
                break;
            } else {
                if(btlData.fileStat.obj.objsize<0){
                    break;
                }
            
                rd_counter--; //Fail could come from the previous lecture or even, from the previous to that one
                rd_counter--;
                printf("Rewinding file to %d\r\n", (rd_counter*512));
                f_lseek(&btlData.fileStat, (rd_counter*512));
                
                btlData.currentState = BOOTLOADER_READ_FILE;      //File opened successfully. Erase Application space 
            }

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
            printf("Err: UKNOWN. Update status: UNKNOWN. Reinstall with PicKit\r\n");
            break;
    }
}

/********************************************************************
* Function: 	ConvertAsciiToHex()
* Input: 		Ascii buffer and hex buffer.
* Overview: 	Converts ASCII to Hex.
********************************************************************/
void ConvertAsciiToHex(uint8_t* asciiRec, uint8_t* hexRec)
{
	uint8_t i = 0;
	uint8_t k = 0;
	uint8_t hex;
	
	
	while((asciiRec[i] >= 0x30) && (asciiRec[i] <= 0x66))
	{
		// Check if the ascci values are in alpha numeric range.
		
		if(asciiRec[i] < 0x3A)
		{
			// Numerical reperesentation in ASCII found.
			hex = asciiRec[i] & 0x0F;
		}
		else
		{
			// Alphabetical value.
			hex = 0x09 + (asciiRec[i] & 0x0F);						
		}
	
		// Following logic converts 2 bytes of ASCII to 1 byte of hex.
		k = i%2;
		
		if(k)
		{
			hexRec[i/2] |= hex;
			
		}
		else
		{
			hexRec[i/2] = (hex << 4) & 0xF0;
		}	
		i++;		
	}		
	
}

/********************************************************************
* Function: 	WriteHexRecord2Flash()
* Overview: 	Writes Hex Records to Flash.
********************************************************************/
uint8_t WriteHexRecord2Flash(uint8_t* HexRecord)
{
	static T_HEX_RECORD HexRecordSt;
	UINT8 Checksum = 0;
	UINT8 i;
	UINT WrData;
	uint32_t ProgAddress;
	//UINT result;
		
	HexRecordSt.RecDataLen = HexRecord[0];
	HexRecordSt.RecType = HexRecord[3];	
	HexRecordSt.Data = &HexRecord[4];	
	
	// Hex Record checksum check.
	for(i = 0; i < HexRecordSt.RecDataLen + 5; i++)
	{
		Checksum += HexRecord[i];
	}	
	
    if(Checksum != 0)
    {
	    //Error. Hex record Checksum mismatch.
	    return CHECKSUM_ERROR;
	} 
	else
	{
		// Hex record checksum OK.
		switch(HexRecordSt.RecType)
		{
			case DATA_RECORD:  //Record Type 00, data record.
				HexRecordSt.Address.byte.MB = 0;
                HexRecordSt.Address.byte.UB = 0;
                HexRecordSt.Address.byte.HB = HexRecord[1];
                HexRecordSt.Address.byte.LB = HexRecord[2];

                // Derive the address.
                HexRecordSt.Address.Val = HexRecordSt.Address.Val + HexRecordSt.ExtLinAddress.Val + HexRecordSt.ExtSegAddress.Val;

                while(HexRecordSt.RecDataLen) // Loop till all bytes are done.
                {

                    // Convert the Physical address to Virtual address. 
                    ProgAddress = (uint32_t)PA_TO_KVA0(HexRecordSt.Address.Val);

                    if(HexRecordSt.RecDataLen < 4)
                    {
                        // Sometimes record data length will not be in multiples of 4. Appending 0xFF will make sure that..
                        // we don't write junk data in such cases.
                        WrData = 0xFFFFFFFF;
                        memcpy(&WrData, HexRecordSt.Data, HexRecordSt.RecDataLen);	
                    }
                    else
                    {	
                        memcpy(&WrData, HexRecordSt.Data, 4);
                    }		
                    // Write the data into flash.	
                    printf("%X in %X\r\n", WrData, ProgAddress);
                    NVM_WordWrite(WrData, ProgAddress);
                    //NVM_RowWrite((uint32_t *)WrData, ProgAddress);								
                    // Increment the address.
                    HexRecordSt.Address.Val += 4;
                    // Increment the data pointer.
                    HexRecordSt.Data += 4;
                    // Decrement data len.
                    if(HexRecordSt.RecDataLen > 3)
                    {
                        HexRecordSt.RecDataLen -= 4;
                    }	
                    else
                    {
                        HexRecordSt.RecDataLen = 0;
                    }	
                }
                break;

			case EXT_SEG_ADRS_RECORD:  // Record Type 02, defines 4th to 19th bits of the data address.
			    HexRecordSt.ExtSegAddress.byte.MB = 0;
				HexRecordSt.ExtSegAddress.byte.UB = HexRecordSt.Data[0];
				HexRecordSt.ExtSegAddress.byte.HB = HexRecordSt.Data[1];
				HexRecordSt.ExtSegAddress.byte.LB = 0;
				// Reset linear address.
				HexRecordSt.ExtLinAddress.Val = 0;
				break;
				
			case EXT_LIN_ADRS_RECORD:   // Record Type 04, defines 16th to 31st bits of the data address. 
				HexRecordSt.ExtLinAddress.byte.MB = HexRecordSt.Data[0];
				HexRecordSt.ExtLinAddress.byte.UB = HexRecordSt.Data[1];
				HexRecordSt.ExtLinAddress.byte.HB = 0;
				HexRecordSt.ExtLinAddress.byte.LB = 0;
				// Reset segment address.
				HexRecordSt.ExtSegAddress.Val = 0;
				break;
				
			case END_OF_FILE_RECORD:  //Record Type 01, defines the end of file record.
				HexRecordSt.ExtSegAddress.Val = 0;
				HexRecordSt.ExtLinAddress.Val = 0;
				
				f_close(&btlData.fileStat);
                printf("Updated completed. Erasing file\r\n");
                //f_unlink(APP_IMAGE_FILE_NAME);
                printf("Reseting\r\n");
                bootloader_ClearTrigger();
                DelayMs(1000);
                run_Application();
				break;
				
			default: 
				HexRecordSt.ExtSegAddress.Val = 0;
				HexRecordSt.ExtLinAddress.Val = 0;
				break;
		}		
	}	
		
    return 0;
}