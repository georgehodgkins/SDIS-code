// declares function to control status LEDs based on state flags
#pragma once
#include "pins.h" // StateFlags enum definition

void update_status_indicators(enum StateFlags);

// controls transfer status LED...status
enum XferStatLED {XFER_LED_OFF, XFER_LED_ON, XFER_LED_BLINK};
void set_xfer_LED (enum XferStatLED);
