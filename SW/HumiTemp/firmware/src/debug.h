#ifndef _DEBUG_H    /* Guard against multiple inclusion */
    #define _DEBUG_H

#define DEBUG

#ifdef DEBUG
    #define DEBUG_PRINT(...) do{ printf( __VA_ARGS__ ); } while( false )
#else
    #define DEBUG_PRINT(...) do{ } while ( false )
#endif

#endif /* _DEBUG_H */