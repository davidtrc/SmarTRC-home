/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

// Based on https://github.com/nyh-workshop/sk1632-fatfs
// Edited by A. Morrison to function on PIC32.
// Ported by Riccardo Leonardi to PIC32MX795F512L  (22/11/2011)
// Many thanks to Aiden Morrison's good work!
// changes: parametrization of SPI port number
// Third port by Ng Y.H. to PIC32MZ-EF microcontrollers. (13/12/2016)
// Using latest version of FatFs!
// NOTE: Timeouts are not tuned!

#include <p32xxxx.h>
//#include <spi.h>
#include "ff.h"
#include "diskio.h"
#include "CustomTime.h"
#include "debug.h"

/* Definitions for MMC/SDC command */
#define CMD0   (0)			/* GO_IDLE_STATE */
#define CMD1   (1)			/* SEND_OP_COND */
#define ACMD41 (41|0x80)	/* SEND_OP_COND (SDC) */
#define CMD8   (8)			/* SEND_IF_COND */
#define CMD9   (9)			/* SEND_CSD */
#define CMD10  (10)			/* SEND_CID */
#define CMD12  (12)			/* STOP_TRANSMISSION */
#define ACMD13 (13|0x80)	/* SD_STATUS (SDC) */
#define CMD16  (16)			/* SET_BLOCKLEN */
#define CMD17  (17)			/* READ_SINGLE_BLOCK */
#define CMD18  (18)			/* READ_MULTIPLE_BLOCK */
#define CMD23  (23)			/* SET_BLOCK_COUNT */
#define ACMD23 (23|0x80)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24  (24)			/* WRITE_BLOCK */
#define CMD25  (25)			/* WRITE_MULTIPLE_BLOCK */
#define CMD41  (41)			/* SEND_OP_COND (ACMD) */
#define CMD55  (55)			/* APP_CMD */
#define CMD58  (58)			/* READ_OCR */

volatile BYTE rtcYear = 111, rtcMon = 11, rtcMday = 22;	// RTC starting values
volatile BYTE rtcHour = 0, rtcMin = 0, rtcSec = 0;

extern unsigned char BRGspeed;

/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

static volatile DSTATUS Stat = STA_NOINIT;	/* Disk status */

static volatile UINT Timer1, Timer2;		/* 1000Hz decrement timer */

static UINT CardType;

static BYTE send_cmd (BYTE cmd,	DWORD arg);

/*-----------------------------------------------------------------------*/
/* Exchange a byte between PIC and MMC via SPI  (Platform dependent)     */
/*-----------------------------------------------------------------------*/

#define xmit_spi(dat) 	xchg_spi(dat)
#define rcvr_spi()		xchg_spi(0xFF)

static BYTE xchg_spi (BYTE dat){
    
    uint8_t recvd = 0;
    static uint8_t dato[1]={0x00};
    dato[0] = dat; 
    SPI3_WriteRead(&dato, 1, &recvd, 1);
    while(SPI3_IsBusy());
	return (BYTE)recvd;
}

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/
static BYTE wait_ready (void){
    
	BYTE res;
    uint32_t tWait = ( GetSystemClock() / 2000 ) * USD_READY_TIMEOUT_MS; //Delay for timeout
    uint32_t tStart = ReadCoreTimer(); 

	rcvr_spi();
	do {
		res = rcvr_spi();
    }
	while ((res != 0xFF) && ((ReadCoreTimer() - tStart) < tWait) );

	return res;
}

/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/
static void deselect (void){
    
	rcvr_spi();
}

/*-----------------------------------------------------------------------*/
/* Select the card and wait ready                                        */
/*-----------------------------------------------------------------------*/
static int select (void){	/* 1:Successful, 0:Timeout */
    
	if (wait_ready() != 0xFF) {
		deselect();
		return 0;
	}
	return 1;
}

/*********************************************************************
 * Function:  		DWORD get_fattime(void)
 * PreCondition:    
 * Input:           None
 * Output:          Time
 * Side Effects:    
 * Overview:        when writing fatfs requires a time stamp
 *					in this exmaple we are going to use a counter
 *					If the starter kit has the 32kHz crystal
 *					installed then the RTCC could be used instead
 * Note:           
 ********************************************************************/
//DWORD get_fattime(void)
//{
//	DWORD tmr;
//
//	//INTDisableInterrupts();
//	tmr =	  (((DWORD)rtcYear - 80) << 25)
//			| ((DWORD)rtcMon << 21)
//			| ((DWORD)rtcMday << 16)
//			| (WORD)(rtcHour << 11)
//			| (WORD)(rtcMin << 5)
//			| (WORD)(rtcSec >> 1);
//	//INTEnableInterrupts();
//
//	return tmr;
//}

/*********************************************************************
 * Function:  		Receive a data packet from MMC 
 * Input:           buff: Data buffer to store received data; btr: Byte count (must be multiple of 4)
 * Output:          1:OK, 0:Failed         
 ********************************************************************/
static int rcvr_datablock (BYTE *buff, UINT btr){
    
	BYTE token;
    uint32_t tWait = ( GetSystemClock() / 2000 ) * USD_RX_TIMEOUT_MS; //Delay for timeout
    uint32_t tStart = ReadCoreTimer();
    uint8_t dummy = 0xFF;

	do {							/* Wait for data packet in timeout of 100ms */
		token = rcvr_spi();
	} while ((token == 0xFF) && ((ReadCoreTimer() - tStart) < tWait));

	if(token != 0xFE) return 0;		/* If not valid data token, retutn with error */

	do {							/* Receive the data block into buffer */
        SPI3_WriteRead(&dummy, 1, buff++, 1); 
        while(SPI3_IsBusy());
        SPI3_WriteRead(&dummy, 1, buff++, 1); 
        while(SPI3_IsBusy());
        SPI3_WriteRead(&dummy, 1, buff++, 1); 
        while(SPI3_IsBusy());
        SPI3_WriteRead(&dummy, 1, buff++, 1); 
        while(SPI3_IsBusy());
	} while (btr -= 4);
	rcvr_spi();						/* Discard CRC */
	rcvr_spi();

	return 1;						/* Return with success */
}

/*********************************************************************
 * Function:  		Send a command packet to MMC 
 * Input:           cmd: Command byte; arg: Argument
 * Output:          Return with the response value         
 ********************************************************************/
static BYTE send_cmd (BYTE cmd, DWORD arg){
    
	BYTE n, res;

	if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card and wait for ready */
	deselect();
	if (!select()) return 0xFF;

	/* Send command packet */
	xmit_spi(0x40 | cmd);			/* Start + Command index */
	xmit_spi((BYTE)(arg >> 24));	/* Argument[31..24] */
	xmit_spi((BYTE)(arg >> 16));	/* Argument[23..16] */
	xmit_spi((BYTE)(arg >> 8));		/* Argument[15..8] */
	xmit_spi((BYTE)arg);			/* Argument[7..0] */
	n = 0x01;						/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;		/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;		/* Valid CRC for CMD8(0x1AA) */
	xmit_spi(n);

	/* Receive command response */
	if (cmd == CMD12) rcvr_spi();	/* Skip a stuff byte when stop reading */
	n = 10;							/* Wait for a valid response in timeout of 10 attempts */
	do
		res = rcvr_spi();
	while ((res & 0x80) && --n);

	return res;
}

/*********************************************************************
 * Function:  		Send a data packet to MMC 
 * Input:           buff: 512 byte data block to be transmitted, token: Data token
 * Output:          1:OK, 0:Failed     
 ********************************************************************/
#if _READONLY == 0
static int xmit_datablock (const BYTE *buff, BYTE token){
	
    BYTE resp;
	UINT bc = 512;

	if (wait_ready() != 0xFF) return 0;

	xmit_spi(token);		/* Xmit a token */
	if (token != 0xFD) {	/* Not StopTran token */
		do {						/* Xmit the 512 byte data block to the MMC */
			xmit_spi(*buff++);
			xmit_spi(*buff++);
		} while (bc -= 2);
		xmit_spi(0xFF);				/* CRC (Dummy) */
		xmit_spi(0xFF);
		resp = rcvr_spi();			/* Receive a data response */
		if ((resp & 0x1F) != 0x05)	/* If not accepted, return with error */
			return 0;
	}

	return 1;
}
#endif	/* _READONLY */

/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/
/*********************************************************************
 * Function:  		Write Sector(s)   
 * Input:           drv:Physical drive number (0), buff: Pointer to the data to be written, sector: Start sector number (LBA), count: Sector count (1..255)
 * Output:          1:OK, 0:Failed     
 ********************************************************************/
#if _READONLY == 0
DRESULT disk_write (BYTE drv, const BYTE *buff, DWORD sector, UINT count){
    
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (Stat & STA_PROTECT) return RES_WRPRT;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	if (count == 1) {		/* Single block write */
		if ((send_cmd(CMD24, sector) == 0)	/* WRITE_BLOCK */ && xmit_datablock(buff, 0xFE))
			count = 0;
	}
	else {				/* Multiple block write */
		if (CardType & CT_SDC) send_cmd(ACMD23, count);
		if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
				count = 1;
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (BYTE pdrv){
    
	BYTE n, cmd, ty, ocr[4];
    uint8_t uSD_switch;
    uint32_t tWait = ( GetSystemClock() / 2000 ) * USD_INIT_TIMEOUT_MS; //Delay for timeout
    
    uSD_switch = MICROSD_SW_Get();
    if(uSD_switch){ /* No card in the socket */
        Stat |= STA_NODISK;
        return Stat;
    } else {
       Stat &= ~STA_NODISK; 
    }

    SPI3_Set_Baudrate(400000);

	for (n = 10; n; n--) rcvr_spi();	/* 80 dummy clocks */

    uint32_t tStart = ReadCoreTimer(); 
        
	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {			/* Enter Idle state */
        DEBUG_PRINT("1");
		if (send_cmd(CMD8, 0x1AA) == 1) {	/* SDv2? */
            DEBUG_PRINT("2");
			for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();			/* Get trailing return value of R7 resp */
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) {				/* The card can work at vdd range of 2.7-3.6V */
                DEBUG_PRINT("3");
				while ( ((ReadCoreTimer() - tStart) < tWait) && send_cmd(ACMD41, 0x40000000));	/* Wait for leaving idle state (ACMD41 with HCS bit) */
				if ( ((ReadCoreTimer() - tStart) < tWait) && send_cmd(CMD58, 0) == 0) {			/* Check CCS bit in the OCR */
                    DEBUG_PRINT("4");
					for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
					ty = (ocr[0] & 0x40) ? CT_SD2|CT_BLOCK : CT_SD2;	/* SDv2 */
				}
			}
		} else {							/* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1) 	{
                DEBUG_PRINT("5");
				ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
			} else {
                DEBUG_PRINT("6");
				ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
			}
			while (((ReadCoreTimer() - tStart) < tWait) && send_cmd(cmd, 0));		/* Wait for leaving idle state */
			if ( !((ReadCoreTimer() - tStart) < tWait) || send_cmd(CMD16, 512) != 0)	/* Set read/write block length to 512 */
                DEBUG_PRINT("7");
				ty = 0;
		}
	}
    DEBUG_PRINT("8");
    
	CardType = ty;
	deselect();

	if (ty) {			/* Initialization succeded */
		Stat &= ~STA_NOINIT;	/* Clear STA_NOINIT */
        SPI3_Set_Baudrate(1000000); 
	}

	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (BYTE pdrv){
	return Stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0) */
	BYTE *buff,		/* Pointer to the data buffer to store read data */
	DWORD sector,	/* Start sector number (LBA) */
	UINT count		/* Sector count (1..255) */
)
{
	if (drv || !count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	if (!(CardType & CT_BLOCK)) sector *= 512;	/* Convert to byte address if needed */

	if (count == 1) {		/* Single block read */
		if ((send_cmd(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
			&& rcvr_datablock(buff, 512))
			count = 0;
	}
	else {				/* Multiple block read */
		if (send_cmd(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
			do {
				if (!rcvr_datablock(buff, 512)) break;
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);				/* STOP_TRANSMISSION */
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive data block */
)
{
	DRESULT res;
	BYTE n, csd[16], *ptr = buff;
	DWORD csize;


	if (drv) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	res = RES_ERROR;
	switch (ctrl) {
		case CTRL_SYNC :	/* Flush dirty buffer if present */
			if (select()) {
				deselect();
				res = RES_OK;
			}
			break;

		case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (WORD) */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
				if ((csd[0] >> 6) == 1) {	/* SDv2? */
					csize = csd[9] + ((WORD)csd[8] << 8) + 1;
					*(DWORD*)buff = (DWORD)csize << 10;
				} else {					/* SDv1 or MMCv2 */
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
					*(DWORD*)buff = (DWORD)csize << (n - 9);
				}
				res = RES_OK;
			}
			break;

		case GET_SECTOR_SIZE :	/* Get sectors on the disk (WORD) */
			*(WORD*)buff = 512;
			res = RES_OK;
			break;

		case GET_BLOCK_SIZE :	/* Get erase block size in unit of sectors (DWORD) */
			if (CardType & CT_SD2) {	/* SDv2? */
				if (send_cmd(ACMD13, 0) == 0) {		/* Read SD status */
					rcvr_spi();
					if (rcvr_datablock(csd, 16)) {				/* Read partial block */
						for (n = 64 - 16; n; n--) rcvr_spi();	/* Purge trailing data */
						*(DWORD*)buff = 16UL << (csd[10] >> 4);
						res = RES_OK;
					}
				}
			} else {					/* SDv1 or MMCv3 */
				if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {	/* Read CSD */
					if (CardType & CT_SD1) {	/* SDv1 */
						*(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
					} else {					/* MMCv3 */
						*(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
					}
					res = RES_OK;
				}
			}
			break;

		case MMC_GET_TYPE :		/* Get card type flags (1 byte) */
			*ptr = CardType;
			res = RES_OK;
			break;

		case MMC_GET_CSD :	/* Receive CSD as a data block (16 bytes) */
			if ((send_cmd(CMD9, 0) == 0)	/* READ_CSD */
				&& rcvr_datablock(buff, 16))
				res = RES_OK;
			break;

		case MMC_GET_CID :	/* Receive CID as a data block (16 bytes) */
			if ((send_cmd(CMD10, 0) == 0)	/* READ_CID */
				&& rcvr_datablock(buff, 16))
				res = RES_OK;
			break;

		case MMC_GET_OCR :	/* Receive OCR as an R3 resp (4 bytes) */
			if (send_cmd(CMD58, 0) == 0) {	/* READ_OCR */
				for (n = 0; n < 4; n++)
					*((BYTE*)buff+n) = rcvr_spi();
				res = RES_OK;
			}
			break;

		case MMC_GET_SDSTAT :	/* Receive SD statsu as a data block (64 bytes) */
			if (send_cmd(ACMD13, 0) == 0) {	/* SD_STATUS */
				rcvr_spi();
				if (rcvr_datablock(buff, 64))
					res = RES_OK;
			}
			break;

		default:
			res = RES_PARERR;
	}

	deselect();

	return res;
}

