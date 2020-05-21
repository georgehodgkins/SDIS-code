// this header initializes areas in RAM and FRAM for log data,
// and declares logging-related functions
#ifndef SDIS_LOG_HDR
#define SDIS_LOG_HDR 1
#include <stdint.h> // fixed-width integers

// these values must be multiples of four less than 2^16
// also consider specific device memory sizes
#define LOG_BUFSIZE 128 // sector size divided by 4
#define LOG_SLABSIZE 16384

// declare buffer and FS slab (defined in logging.c)
extern uint32_t logbuf[LOG_BUFSIZE]; // each entry is 64 bits
extern uint8_t fs_slab[LOG_SLABSIZE];

// initialization function
void setup_logging(void);

// new log entry
void log (uint32_t);

// RAM -> FRAM
void dump_logbuf (void);

// FRAM -> SDC
void dump_logstore (void);
#endif
