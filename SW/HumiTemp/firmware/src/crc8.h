#ifndef _CRC8_H    /* Guard against multiple inclusion */
    #define _CRC8_H

#include <stdint.h>
#include <stddef.h>

#define POLYNOMIAL1    (0x1070U<<3)
#define POLYNOMIAL2    (0x31)

uint8_t Crc8Block(uint8_t *data, uint8_t len, uint16_t polinomial, uint8_t init_val);

#endif /* _CRC8_H */
