#ifndef _DEBUG_H    /* Guard against multiple inclusion */
    #define _DEBUG_H

    #include <stdio.h>
    #include <string.h>

#define DEBUG

#ifdef DEBUG
    #define DEBUG_PRINT(...) do{ printf( __VA_ARGS__ ); printf("\r");} while( false )
#else
    #define DEBUG_PRINT(...) do{ } while ( false )
#endif

#endif /* _DEBUG_H */