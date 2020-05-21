#include "driverlib.h"
#include "logging.h"
#include "filesys/ff.h"
#include "rtc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// log filename format: sdis-log-hhhhhhhh (timestamp in hex)

// allocate log buffer in RAM
#pragma NOINIT (logbuf)
uint32_t logbuf [LOG_BUFSIZE];

// next buffer entry index
uint16_t logbuf_pt;

// allocate slab for filesystem in FRAM
#pragma PERSISTENT(fs_slab)
uint8_t fs_slab [LOG_SLABSIZE] = {0};

// filesystem object for FRAM FS
FATFS fram_fs = {0};

// full flag for filesystem
BYTE fram_full;

// initialize RAM buffer and FRAM filesystem for logging data
void setup_logging(void) {
    // initialize RAM buffer and counter
    logbuf_pt = 0;
    int i;
    for (i = 0; i < LOG_BUFSIZE; ++i) {
        logbuf[i] = 0;
    }

    // setup FRAM
    // no wait states necessary since we're using a slow clock
    FRAMCtl_configureWaitStateControl(FRAMCTL_ACCESS_TIME_CYCLES_0);
    // wait to power up until written to when exiting LPM
    FRAMCtl_delayPowerUpFromLPM(FRAMCTL_DELAY_FROM_LPM_ENABLE);

    // mount FAT volume in FRAM, creating volume if it does not exist
    if (f_mount(&fram_fs, "0:", 1)) { // nonzero return indicates error
        // allocate 2 KB working buffer for formatting
        char work[2048];
        // format drive with default options
        f_mkfs("0:", NULL, (void*) work, 2048);
        // mount newly allocated drive
        f_mount(&fram_fs, "0:", 1);
    }

    // check for free space
    DWORD free_clusters;
    FATFS* fsobj;
    f_getfree("0:", &free_clusters, &fsobj);
    DWORD free_bytes = free_clusters*(fsobj->csize)*512;
    fram_full = (free_bytes < LOG_BUFSIZE) ? 1 : 0;
}

// create a new log entry
void log (uint32_t entry) {
    // if RAM buffer is full, dump to FRAM
    if (logbuf_pt == LOG_BUFSIZE) {
        dump_logbuf();
    }

    // write entry to RAM buffer
    logbuf[logbuf_pt] = get_timestamp();
    logbuf[logbuf_pt + 1] = entry;
    logbuf_pt += 2;
}

// write RAM buffer to FRAM as a file
void dump_logbuf (void) {

    if (!fram_full) {
        // check free space
        DWORD free_clusters;
        FATFS* fsobj;
        f_getfree("0:/", &free_clusters, &fsobj);
        DWORD free_bytes = free_clusters*(fsobj->csize)*512;
        if (free_bytes < logbuf_pt) {
            fram_full = 1;
        }
    }

    if (fram_full) { // delete oldest entry
        // find oldest (smallest timestamp) entry
        DIR fs_root; // root directory object
        FILINFO file; // file info object
        DWORD min_filetime = 0xFFFFFFFE; // min timestamp tracker
        char min_filename[18]; // min timestamp filename tracker
        char path_to_del[22]; // path to be deleted
        strcpy(path_to_del, "0:/");

        f_opendir (&fs_root, "0:"); // open directory
        f_readdir(&fs_root, &file); // read first file

        while (file.fname) {
            // get timestamp
            DWORD file_time = strtol(&(file.fname[9]), NULL, 16);

            if (file_time < min_filetime) {
                min_filetime = file_time;
                strcpy(min_filename, file.fname);
            }

            // get next file
            f_readdir(&fs_root, &file);
        }
        // close directory
        f_closedir(&fs_root);
        // assemble path
        strcat(path_to_del, min_filename);
        // delete file
        f_unlink(path_to_del);
    }

    // create new filename
    char namebuf[22];
    sprintf(namebuf, "0:/sdis-log-%.8x", (unsigned long) get_timestamp());

    // write to new file
    FIL write_to;
    UINT bytes_written;
    // open file, appending if it already exists
    f_open(&write_to, namebuf, FA_WRITE | FA_OPEN_APPEND);
    // write log buffer to file
    f_write(&write_to, (void*) logbuf, logbuf_pt, &bytes_written);
    f_close(&write_to);
    // reset pointer
    logbuf_pt = 0;
}

// destination path for streamed data
char stream_path[22];
// helper function to stream data from FRAM to the SD card
// without the need for a buffer
//
// assumes the FS is mounted
UINT stream_to_mmc(const BYTE* buff, UINT btf) {
    if (!btf) { // sense call
        return 1; // always ready
    } else { // write data to file
        FIL stream_handle;
        UINT rtn;
        f_open(&stream_handle, stream_path, FA_WRITE | FA_OPEN_APPEND);
        f_write(&stream_handle, (void*) buff, btf, &rtn);
        f_close(&stream_handle);
        return rtn;
    }
}

// write FRAM data to SD card
void dump_logstore (void) {
    FATFS mmc_fs;

    f_mount(&mmc_fs, "1:/", 1);

    DIR fram_root; // directory object representing root of FRAM disk
    FILINFO src_info; // found info on files to transfer
    FIL src_handle; // file handle for source file
    char src_path[22]; // path for file to be copied & deleted from FRAM

    // set up paste and deletion path buffers
    strcpy(stream_path, "1:/");
    strcpy(src_path, "0:/");

    // read first file to transfer
    f_opendir(&fram_root, "1:/");
    f_readdir(&fram_root, &src_info);

    // transfer all available files
    while (src_info.fname) {
        UINT unused;

        // create path names
        strcpy(&stream_path[3], src_info.fname);
        strcpy(&src_path[3], src_info.fname);

        // open source file for reading
        f_open(&src_handle, src_path, FA_READ);

        // stream the file from FRAM to SD card
        // streaming function handles opening and closing dest file
        f_forward(&src_handle, stream_to_mmc, (UINT) src_info.fsize, &unused);

        // close source file
        f_close(&src_handle);

        // delete source file
        f_unlink(src_path);

        // get next file in directory
        f_readdir(&fram_root, &src_info);
    }

    // close directory
    f_closedir(&fram_root);

    // unmount SD card filesystem
    f_mount(NULL, "1:/", 0);
}
