#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#include "controls/ghostfat.h"


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	(void) pdrv;
	/* always OK */
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	(void) pdrv;
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	(void) pdrv;

	for (UINT i = 0; i < count; ++i) {
		ghostfat_read_block(sector + i, buff);
		buff += GHOSTFAT_SECTOR_SIZE;
	}
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
) {
	(void)pdrv;

	for (UINT i = 0; i < count; ++i) {
		ghostfat_write_block(sector + i, buff);
		buff += GHOSTFAT_SECTOR_SIZE;
	}
	return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
) {
	(void) pdrv;
	(void) buff;

	switch(cmd) {
		case CTRL_SYNC:
	        return RES_OK;
		default:
	        return RES_ERROR;
	}
}
