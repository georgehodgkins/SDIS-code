// links sensing functions
#pragma once
#include <stdint.h>

enum VWCStat {VWC_OK = 0, VWC_LOW = 1, VWC_HIGH = 2, VWC_NOFLOW = 3};

enum VWCStat check_vwc (void);

uint8_t sense_daytime(void);

