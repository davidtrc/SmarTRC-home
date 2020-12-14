#ifndef _DEBUG_H    /* Guard against multiple inclusion */
    #define _DEBUG_H

#ifdef DEBUG
    #define DEBUG_PRINT(...) do{ printf( stderr, __VA_ARGS__ ); } while( false )
#else
    #define DEBUG_PRINT(...) do{ } while ( false )
#endif

#endif /* _EXAMPLE_FILE_NAME_H */