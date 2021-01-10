#ifndef _USD_H    /* Guard against multiple inclusion */
    #define _USD_H

#include "definitions.h"                // SYS function prototypes
#include "diskio.h"

bool init_uSD(void);

DSTATUS uSD_Get_Status(void);

FRESULT uSD_Open_File(char *filename, FIL file, BYTE mode);

FRESULT uSD_Close_File(FIL* file);

bool uSD_Check_If_File_Exists(char *filename);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
