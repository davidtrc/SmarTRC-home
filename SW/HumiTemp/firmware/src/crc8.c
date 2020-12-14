#include "Crc8.h"

/****************************************************************************/
/*
 *
 * Based on http://websvn.hylands.org/filedetails.php?repname=Projects&path=%2Fcommon%2FCrc8.c&sc=1
 *
 */
/**
*   Calculates the CRC-8 used as part of SMBus.
*
*   CRC-8 is defined to be x^8 + x^2 + x + 1
*
*   To use this function use the following template:
*
*       crc = Crc8( crc, data );
*/

#if 1   // Traditional implementation

#define POLYNOMIAL    (0x1070U << 3)

unsigned char Crc8(unsigned char inCrc, unsigned char inData){
    
    int i;
    unsigned short data;

    data = inCrc ^ inData;
    data <<= 8;
 
    for ( i = 0; i < 8; i++ ){
        if (( data & 0x8000 ) != 0 ){
            data = data ^ POLYNOMIAL;
        }
            data = data << 1;
    }

    return (unsigned char)( data >> 8 );
} // Crc8

#else   // Optimized for 8 bit CPUs (0x22 bytes on ATMega128 versus 0x30 for above version)

unsigned char Crc8( unsigned char inCrc, unsigned char inData ){
    unsigned char   i;
    unsigned char   data;

    data = inCrc ^ inData;
 
    for ( i = 0; i < 8; i++ ){
        if (( data & 0x80 ) != 0 ){
            data <<= 1;
            data ^= 0x07;
        } else {
            data <<= 1;
        }
    }

    return data;
} // Crc8

#endif

/****************************************************************************/
/**
*   Calculates the CRC-8 used as part of SMBus over a block of memory.
*/

uint8_t Crc8Block(uint8_t *data, uint8_t len ){
    
    uint8_t crc_temp=0;
    
    while ( len > 0 )
    {
        crc_temp = Crc8(crc_temp, *data++ );
        len--;
    }

    return crc_temp;

} // Crc8Block
