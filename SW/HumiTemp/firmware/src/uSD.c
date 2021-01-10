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
    
    if(f_mount(&Fatfs, "", 0) != FR_OK){         //Mounting the file system
        DEBUG_PRINT("Invalid file system");
        SD_stat |= STA_NOTMOUNTED;
        return FR_INVALID_DRIVE;
    }
    
    return true;
}

DSTATUS uSD_Get_Status(void){
    return SD_stat;
}

FRESULT uSD_Open_File(char *filename, FIL file, BYTE mode){
    
    return f_open(&file, filename, mode); ;
}

FRESULT uSD_Close_File(FIL* file){
    
    return f_close(file);
}

bool uSD_Check_If_File_Exists(char *filename){
    
    FIL temp_file;
    FRESULT fr;
    
    fr = f_open(&temp_file, filename, FA_READ);
    
    if(!fr){
        f_close(&temp_file);
        return true;
    }
    
    return false;
}

/* *****************************************************************************
 End of File
 */
