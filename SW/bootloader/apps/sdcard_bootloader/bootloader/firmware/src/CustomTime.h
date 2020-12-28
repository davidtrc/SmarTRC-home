
#ifndef _CUSTOM_TIME_H    /* Guard against multiple inclusion */
    #define _CUSTOM_TIME_H

#include <xc.h>

#include "definitions.h"                // SYS function prototypes

/* Provide C++ Compatibility */
#ifdef __cplusplus
    extern "C" {
#endif

#define GetSystemClock()    (200000000ULL) 
#define ReadCoreTimer()     _CP0_GET_COUNT()           // Read the MIPS Core Timer
        
void DelayUs(uint32_t delay_in_us);
void DelayMs(uint32_t delay_in_ms);

/* Provide C++ Compatibility */
#ifdef __cplusplus
    }
#endif

#endif /* _EXAMPLE_FILE_NAME_H */
