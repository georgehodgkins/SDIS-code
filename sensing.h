// sensing functions and enums
#pragma once
#include <stdint.h>
#include "pins.h"


enum VWCStat {VWC_OK = 0, // VWC is between PWP and FC
    VWC_LOW = 1, // VWC is near or below PWP
    VWC_HIGH = 2,// VWC is near or above FC
    VWC_NOFLOW = 3 // VWC does not seem to be increasing, even though the valve is open
};

// returns VWC statuses (see above) based on sensor reading
enum VWCStat check_vwc (enum StateFlags);

// returns 1 if solar panel voltage is above the daytime threshold, 0 if below
uint8_t sense_daytime(void);

