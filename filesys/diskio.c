/*-----------------------------------------------------------------------
 * Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014
 * Filled in to use TI MMC card hardware interface
 *-----------------------------------------------------------------------*/

#include "diskio.h"
#include "mmc/mmc.h"

// this should not be > 255
#define INITIALIZATION_TIMEOUT 200


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

// size of currently initialized MMC card
unsigned long diskio_card_size;

DSTATUS disk_initialize (void)
{
	char card_stat = 0x1;
	BYTE iter = 0;
	// try to initialize card
	while (card_stat != MMC_SUCCESS) {
	    card_stat = mmcInit();
	    ++iter;
	    if (iter == INITIALIZATION_TIMEOUT) {
	        return STA_NODISK; // initialization timed out
	    }
	}

    // check that interface is stable
	iter = 0;
    while (mmcPing() != MMC_SUCCESS) {
        ++iter;
        if (iter == INITIALIZATION_TIMEOUT) {
            return STA_NOINIT; // disk is bad or interface is not stable
        }
    }

    // get card size
    diskio_card_size = mmcReadCardSize();

    // initialization successful
    return STA_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Partial Sector
 * 512 byte sectors                                                  */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp (
	BYTE* buff,		/* Pointer to the destination object */
	DWORD sector,	/* Sector number (LBA) */
	UINT offset,	/* Offset in the sector */
	UINT count		/* Byte count (bit15:destination) */
)
{
	// check parameters for validity
    if (offset > 512 || count > (512 - offset) ||
            sector*512 > diskio_card_size) {
        return RES_PARERR;
    }

    // calculate address with offset
    UINT src_addr = sector*512 + offset;

    // call MMC library read function
    char read_stat = mmcReadBlock(src_addr, count, buff);

    // check for read errors
    if (read_stat != MMC_SUCCESS) {
        return RES_ERROR;
    } else {
        return RES_OK;
    }
}



/*-----------------------------------------------------------------------
* Write Partial Sector
 * For each write, the first call sets
 * the destination address on a sector boundary, and subsequent calls write
 * a block each (using read-modify-write if necessary)
 *-----------------------------------------------------------------------*/

DRESULT disk_writep (
	BYTE* buff,		/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
	DWORD sc		/* Sector number (LBA) or Number of bytes to send */
)
{
    //
    static UINT dest_ptr = 0;

    if (dest_ptr > diskio_card_size) {
        // might want to reset dest_ptr in here as well
        return RES_PARERR;
    }

	if (!buff) {
		if (sc) { // initialization
            dest_ptr = sc*512;
            if (dest_ptr > diskio_card_size) {
                    // might want to reset dest_ptr in here as well
                    return RES_PARERR;
            }
		} else { // finalization
			dest_ptr = 0;
		}
	} else { // block write
	    if (sc != 512) { // non-aligned write, error
	        return RES_PARERR;
	    }
	    // call MMC lib
	    char write_stat = mmcWriteBlock(dest_ptr, sc, buff);

	    // check for errors
	    if (write_stat != MMC_SUCCESS) {
	        return RES_ERROR;
	    }

	    // advance destination pointer
	    dest_ptr += sc;
	}
	return RES_OK;
}

