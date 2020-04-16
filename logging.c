#pragma once
#include "driverlib.h"
#include "logging.h"

// initialize RAM buffer and FRAM to hold log data
void setup_logging(void) {
    // initialize RAM buffer and counter
    logbuf_pt = 0;
    int i;
    for (i = 0; i < LOGBUFSIZE; ++i) {
        logbuf[i] = 0;
    }

    // setup FRAM
    // no wait states necessary since we're using a slow clock
    FRAMCtl_configureWaitStateControl(FRAMCTL_ACCESS_TIME_CYCLES_0);
    // wait to power up until written to when exiting LPM
    FRAMCtl_delayPowerUpFromLPM(FRAMCTL_DELAY_FROM_LPM_ENABLE);

    // initialize log store in FRAM and counter
    logstore_pt = 0;
    FRAMCtl_fillMemory32(0, logstore, LOGSTORESIZE);
}

// write log buffer in RAM to FRAM
void write_log_buffer (void) {
    // wrap log entries back to the beginning if we run out of nonvolatile store space
    if ((logstore_pt + logbuf_pt) > LOGSTORESIZE) {
        // space remaining at end of log store
        int16_t end_chunk = LOGSTORESIZE - logstore_pt;
        // writes that will be wrapped around
        int16_t beg_chunk = logstore_pt + logbuf_pt - end_chunk;

        // write end chunk to the end of log store
        FRAMCtl_write32(logbuf, (uint32_t*) logstore + logstore_pt, end_chunk);

        // wrap the rest around to the beginning of the store
        FRAMCtl_write32( (uint32_t*) logbuf + end_chunk, logstore, beg_chunk);

        // update counters
        logstore_pt = beg_chunk;
        logbuf_pt = 0;
    } else {
        // write log buffer to store
        FRAMCtl_write32(logbuf, (uint32_t*) logstore + logstore_pt, logbuf_pt);

        // update counters
        logstore_pt += logbuf_pt;
        logbuf_pt = 0;
    }
}

// store log entry in log
void log_entry (uint32_t timestamp, uint32_t event) {
    // if buffer is full, dump it to persistent memory first
    // this will also reset the counter
    if (logbuf_pt == LOGBUFSIZE) {
        write_log_buffer();
    }

    // store entry in buffer
    logbuf[logbuf_pt] = timestamp;
    logbuf[logbuf_pt + 1] = event;

    // update counter
    logbuf_pt += 2;
}
