#ifndef _ZIGBEE_H    /* Guard against multiple inclusion */
    #define _ZIGBEE_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#define ZIGBEE_WR_SHORT_RQST_BYTES_SIZE          1       //Bytes of a short write request
#define ZIGBEE_WR_LONG_RQST_BYTES_SIZE           2       //Bytes of a long write request
#define ZIGBEE_RD_SHORT_RQST_BYTES_SIZE          1       //Bytes of a short read request
#define ZIGBEE_RD_LONG_RQST_BYTES_SIZE           2       //Bytes of a long read request
#define ZIGBEE_RESPONSE_BYTES_SIZE               1       //Bytes of response (long and short register)
    
#define ZIGBEE_RD_BIT                           0       //Last bit of a read petition
#define ZIGBEE_WR_BIT                           1       //Last bit of a write petition

#define ZIGBEE_SHORT_RD_MSG_BYTES_SIZE         ZIGBEE_RD_SHORT_RQST_BYTES_SIZE+ZIGBEE_RESPONSE_BYTES_SIZE //Total size of a short register memory read (include request(1) and response(1))
#define ZIGBEE_LONG_RD_MSG_BYTES_SIZE          ZIGBEE_RD_LONG_RQST_BYTES_SIZE+ZIGBEE_RESPONSE_BYTES_SIZE  //Total size of a long register memory read (include request(2) and response(1))

#define ZIGBEE_SHORT_WR_MSG_BYTES_SIZE         ZIGBEE_WR_SHORT_RQST_BYTES_SIZE+ZIGBEE_RESPONSE_BYTES_SIZE //Total size of a short register memory read (include request(1) and response(1))
#define ZIGBEE_LONG_WR_MSG_BYTES_SIZE          ZIGBEE_WR_LONG_RQST_BYTES_SIZE+ZIGBEE_RESPONSE_BYTES_SIZE  //Total size of a long register memory read (include request(2) and response(1))
    
#define ZIGBEE_TXMCR_REG                    0x11

uint8_t ZigBee_Get_Short_Register_Value(uint8_t register_in);

bool ZigBee_Set_Short_Register_Value(uint8_t register_in, uint8_t reg_val, bool check_correct_write);

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
