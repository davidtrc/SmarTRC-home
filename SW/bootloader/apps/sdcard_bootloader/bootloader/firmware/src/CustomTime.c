#include <xc.h>

#include "definitions.h"                // SYS function prototypes
#include "CustomTime.h"

void DelayUs(uint32_t delay_in_us){ 
    uint32_t tWait = ( GetSystemClock() / 2000000 ) * delay_in_us; 
    uint32_t tStart = ReadCoreTimer(); 
    while( ( ReadCoreTimer() - tStart ) < tWait ); 
} 
 
void DelayMs(uint32_t delay_in_ms){
    uint32_t tWait = ( GetSystemClock() / 2000 ) * delay_in_ms;
    uint32_t tStart = ReadCoreTimer();
    while( ( ReadCoreTimer() - tStart ) < tWait );
}