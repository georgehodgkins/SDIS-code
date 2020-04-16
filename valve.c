// valve actuation functions
#include "valve.h"
#include "pins.h" // pin mappings
#include "driverlib.h" // TI driver library

// drive the valve open pin high
void open_valve (void) {
    GPIO_setOutputHighOnPin(VALVEDRIVEPORT, VALVEDRIVEPIN);
}

// drive the valve open pin low
void close_valve (void) {
    GPIO_setOutputLowOnPin(VALVEDRIVEPORT, VALVEDRIVEPIN);
}
