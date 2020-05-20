/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "driverlib.h"
#include "mmc/mmc.h"
#include "pins.h"
#include "logging.h"

/* Definitions of physical drive number for each drive */
#define DEV_FRAM	0	// Map FS in FRAM to drive 0
#define DEV_MMC		1	// Map external card to drive 1


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    switch(pdrv) {
    case DEV_FRAM:
        // FRAM not powered on
        if (!(GCCTL0 & FRPWR) ||
                // FRAM broken or not configured correctly
                FRAMCtl_getInterruptStatus(FRAMCTL_ACCESS_TIME_ERROR_FLAG |
                                           FRAMCTL_UNCORRECTABLE_BIT_FLAG)) {
            return STA_NOINIT;
        } else {
            return STA_OK;
        }
    case DEV_MMC:
        // check if card detect input is high
        if (!GPIO_getInputPinValue(SDCCDPORT, SDCCDPIN)) {
            return STA_NODISK;
        } else if (mmcGoIdle()) { // non-zero return indicates error
            return STA_NOINIT;
        } else {
            return STA_OK;
        }
    default:
        return STA_NODISK;
    }
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	switch(pdrv) {
	case DEV_FRAM:
	    // clear interrupts, then power on
	    GCCTL0 &= ~(ACCTEIFG | UBDIFG | CBDIFG);
	    GCCTL0 |= FRPWR;
	    // wait for a bit, then check status
	    __delay_cycles(16);
	    return disk_status(DEV_FRAM);
	case DEV_MMC:
	    // check CD pin
	    if ((!GPIO_getInputPinValue(SDCCDPORT, SDCCDPIN))) {
	        return STA_NODISK;
	    } else if (mmcInit()) { // nonzero return indicates error
	        return STA_NOINIT;
	    } else {
	        return STA_OK;
	    }
	default:
	    return STA_NODISK;
	}
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
    // check that the requested disk exists and is ready
    if (pdrv > 1) {
        return RES_PARERR;
    } else if (disk_status(pdrv)) {
        return RES_NOTRDY;
    }

    unsigned int i,s; // for loop counters

    switch(pdrv) {
    case DEV_FRAM:
        // copy data from FRAM to (presumably) RAM
        for (s = sector; s < sector + count; ++s) {
            for (i = 0; i < 512; ++i) {
                buff[i] = fs_slab[512*s + i];
            }
            buff += 512;
            if (FRAMCtl_getInterruptStatus(FRAMCTL_UNCORRECTABLE_BIT_FLAG)) {
                return RES_ERROR;
            }
        }
        return RES_OK;
    case DEV_MMC:
        for (s = sector; s < sector + count; ++s) {
            if (mmcReadSector(s, buff)) { // nonzero return indicates error
                return RES_ERROR;
            }
            buff += 512;
        }
        return RES_OK;
    default:
        return RES_PARERR;
    }
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
    // check that the requested disk exists and is ready
    if (pdrv > 1) {
        return RES_PARERR;
    } else if (disk_status(pdrv)) {
        return RES_NOTRDY;
    }

    unsigned int s; // for loop counter

    switch (pdrv) {
    case DEV_FRAM:
        for (s = sector; s < sector + count; ++s) {
            FRAMCtl_write8((uint8_t*) buff, &fs_slab[s*512], 512);
            if (FRAMCtl_getInterruptStatus(FRAMCTL_UNCORRECTABLE_BIT_FLAG)) {
                return RES_ERROR;
            }
        }
        return RES_OK;
    case DEV_MMC:
        for (s = sector; s < sector + count; ++s) {
            if (mmcWriteSector(s, (unsigned char*) buff)) { // nonzero return indicates error
                return RES_ERROR;
            }
        }
        return RES_OK;
    default:
        return RES_PARERR;
    }
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    // check that the requested disk exists and is ready
    if (pdrv > 1) {
        return RES_PARERR;
    } else if (disk_status(pdrv)) {
        return RES_NOTRDY;
    }

    // Only the first 5 commands need to be implemented for FatFS
    switch (pdrv) {
    case DEV_FRAM:

        switch (cmd) {
        case CTRL_SYNC: // complete pending writes, N/A here unless DMA is used
            return RES_OK;
        case GET_SECTOR_COUNT: // number of sectors available
            *((LBA_t*) buff) = LOG_SLABSIZE/512;
            return RES_OK;
        case GET_SECTOR_SIZE: // size of a sector in bytes
            *((WORD*) buff) = 512;
            return RES_OK;
        case GET_BLOCK_SIZE: // erase block size (Flash only, return 1)
            *((DWORD*) buff) = 1;
            return RES_OK;
        case CTRL_TRIM: // trim a block of sectors (Flash only)
            return RES_OK;
        default: // invalid command
            return RES_PARERR;
        }

    case DEV_MMC:
        // see comments above, considerations are similar
        switch(cmd) {
        case CTRL_SYNC:
            return RES_OK;
        case GET_SECTOR_COUNT:
            *((LBA_t*) buff) = mmcReadCardSize()/512; // I...hope this function is denominated in bytes
            return RES_OK;
        case GET_SECTOR_SIZE:
            *((WORD*) buff) = 512;
            return RES_OK;
        case GET_BLOCK_SIZE:
            *((DWORD*) buff) = 1;
            return RES_OK;
        case CTRL_TRIM:
            return RES_OK;
        default:
            return RES_PARERR;
        }

    default:
        return RES_PARERR; // invalid drive ID
    }
}

