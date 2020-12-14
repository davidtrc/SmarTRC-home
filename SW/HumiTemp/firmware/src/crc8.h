#ifndef _CRC8_H    /* Guard against multiple inclusion */
    #define _CRC8_H

#include <stdint.h>
#include <stddef.h>

uint8_t Crc8Block(uint8_t *data, uint8_t len );

#endif /* _CRC8_H */
