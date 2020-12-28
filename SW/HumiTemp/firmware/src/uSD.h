#ifndef _USD_H    /* Guard against multiple inclusion */
    #define _USD_H

#include "definitions.h"                // SYS function prototypes
#include "diskio.h"

bool init_uSD(void);

DSTATUS uSD_Get_Status(void);

bool Load_SD_DDBB(settings_t *global_settings);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
