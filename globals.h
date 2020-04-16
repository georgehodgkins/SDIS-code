#pragma once
#include <stdint.h>

// This file contains definitions of global variables and an initializer function

void init_globals(void);

// flags indicating which interrupt was triggered
enum IntFlags {NONE = 0x00, RTC = 0x01, POWER = 0x02, SERIAL = 0x04};

// current interrupt status read by main loop
enum IntFlags interrupt_flags;

// counter for RTC rollovers
uint16_t rtc_count;
