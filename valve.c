#include "valve.h"
#include "pins.h"
#include "driverlib.h"

void open_valve (void) {
    GPIO_setOutputHighOnPin(VALVEDRIVEPORT, VALVEDRIVEPIN);
}

void close_valve (void) {
    GPIO_setOutputLowOnPin(VALVEDRIVEPORT, VALVEDRIVEPIN);
}
