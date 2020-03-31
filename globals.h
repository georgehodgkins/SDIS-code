#pragma once
#include <stdint.h>

// This file contains definitions of global variables and an initializer function

void init_globals(void);

// when an interrupt is triggered, it will set the appropriate flag and then return to the main loop
enum IntFlags {NONE = 0x00, RTC = 0x01, POWER = 0x02, SERIAL = 0x04};

enum IntFlags interrupt_flags;

// counter for RTC rollovers
uint16_t rtc_count;
