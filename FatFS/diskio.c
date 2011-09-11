/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */

DSTATUS disk_initialize (
	BYTE drv				/* Physical drive number (0..) */
)
{
	return FR_OK;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */

DSTATUS disk_status (
	BYTE drv		/* Physical drive number (0..) */
)
{
	return FR_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive number (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	uint8_t ErrorCode = RES_OK;

	if (USB_HostState != HOST_STATE_Configured)
	  ErrorCode = RES_NOTRDY;
	else if (MS_Host_ReadDeviceBlocks(&Datalogger_MS_Interface, 0, sector, count, 512, buff))
	  ErrorCode = RES_ERROR;

	return ErrorCode;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive number (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	uint8_t ErrorCode = RES_OK;

	if (USB_HostState != HOST_STATE_Configured)
	  ErrorCode = RES_NOTRDY;
	else if (MS_Host_WriteDeviceBlocks(&Datalogger_MS_Interface, 0, sector, count, 512, buff))
	  ErrorCode = RES_ERROR;

	return RES_OK;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive number (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	if (ctrl == CTRL_SYNC)
	  return RES_OK;
	else
	  return RES_PARERR;
}


DWORD get_fattime (void)
{
	struct
	{
		uint8_t Hour;
		uint8_t Minute;
		uint8_t Second;
		uint8_t Day;
		uint8_t Month;
		uint8_t Year;
	} CurrTimeDate;
	
	CurrTimeDate.Year   = 1;
	CurrTimeDate.Month  = 1;
	CurrTimeDate.Day    = 1;
	CurrTimeDate.Hour   = 1;
	CurrTimeDate.Minute = 1;
	CurrTimeDate.Second = 1;

	return ((DWORD)(20 + CurrTimeDate.Year) << 25) |
	             ((DWORD)CurrTimeDate.Month << 21) |
	               ((DWORD)CurrTimeDate.Day << 16) |
	              ((DWORD)CurrTimeDate.Hour << 11) |
	             ((DWORD)CurrTimeDate.Minute << 5) |
	      (((DWORD)CurrTimeDate.Second >> 1) << 0);
}

