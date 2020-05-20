// this header initializes areas in RAM and FRAM for log data,
// and declares logging-related functions
#ifndef SDIS_LOG_HDR
#define SDIS_LOG_HDR 1
#include <stdint.h> // fixed-width integers

// these values must be multiples of four less than 2^16
// also consider specific device memory sizes
#define LOG_BUFSIZE 256
#define LOG_SLABSIZE 8192

// declare buffer and FS slab (defined in logging.c)
extern uint8_t logbuf[LOG_BUFSIZE];
extern uint8_t fs_slab[LOG_SLABSIZE];

// initialization function
//void setup_logging(void);

// write logbuf in RAM to persistent memory (FRAM)
//void write_log_buffer (void);
#endif
