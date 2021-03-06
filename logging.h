// this header initializes areas in RAM and FRAM for log data,
// and declares logging-related functions
#pragma once
#include <stdint.h> // fixed-width integers

// these values must be multiples of two less than 2^16
// also consider specific device memory sizes
#define LOGBUFSIZE 256
#define LOGSTORESIZE 8192

// log buffer in RAM
#pragma NOINIT (logbuf)
uint32_t logbuf [LOGBUFSIZE];

// next buffer entry index
uint16_t logbuf_pt;

// log file in FRAM/Flash
#pragma PERSISTENT(logstore)
uint32_t logstore [LOGSTORESIZE] = {0};

// next log store entry index
uint16_t logstore_pt;

// initialization function
void setup_logging(void);

// write logbuf in RAM to persistent memory (FRAM)
void write_log_buffer (void);
