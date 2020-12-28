#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdio.h>
#include <string.h>

#include "definitions.h"                // SYS function prototypes
#include "diskio.h"
#include "ff.h"
#include "debug.h"

static FATFS Fatfs;
static DSTATUS SD_stat;

bool init_uSD(void){
    DSTATUS sd_init = disk_initialize(0);
    SD_stat = sd_init;
    if(sd_init){
        DEBUG_PRINT("No card or card error. Err=%d", sd_init);
        return false;
    }
    
    if(f_mount(&Fatfs, "", 0) == FR_INVALID_DRIVE){         //Mounting the file system
        DEBUG_PRINT("Invalid file system");
        SD_stat |= STA_NOTMOUNTED;
        return FR_INVALID_DRIVE;
    }
    
    return true;
}

DSTATUS uSD_Get_Status(void){
    return SD_stat;
}

bool Load_SD_DDBB(settings_t *global_settings){
    
    
    return true;
}

    /****************************** TEST DE SD*****************************************/

/*
    //Initialize all MPLAB Harmony modules, including application(s).
    char buffer[256] = { 0 };
    const char text1[] = "Hello World!\r This file is created with FatFs driver!\r Have a great day!\r\r";
    unsigned int numWrite = 0;   // number of bytes written to file.
    unsigned int numRead = 0;    // number of bytes read from file. 
    
 
    FATFS *fs; // Pointer to file system object
    FIL file1;     
        
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
    
    while ( !btn_test2_interrupt )
    {
        //Maintain state machines of all polled MPLAB Harmony modules
        //SYS_Tasks ( );
        LED_TEST_1_Toggle();
        LED_TEST_2_Toggle();
        DelayMs(500);
    }

    btn_test2_interrupt = false;
    
*/  
    /****************************** FIN TEST DE SD*****************************************/

/* *****************************************************************************
 End of File
 */
