// function to control status LEDs based on state flags
#include "pins.h"
#include "driverlib.h"
#include <stdint.h>

// update status LEDs based on state flags
void update_status_indicators (enum StateFlags state) {
    // LED indicating if valve is open
    if (state & VOPEN) {
        GPIO_setOutputHighOnPin(STATUSPORT, VOPENPIN);
    } else {
        GPIO_setOutputLowOnPin(STATUSPORT, VOPENPIN);
    }

    // LED indicating if valve will open once night comes
    if (state & VQUEUED) {
        GPIO_setOutputHighOnPin(STATUSPORT, VQUEUEDPIN);
    } else {
        GPIO_setOutputLowOnPin(STATUSPORT, VQUEUEDPIN);
    }

    // LED indicating if there is a flow error
    if (state & FLOW_ERR) {
        GPIO_setOutputHighOnPin(STATUSPORT, FLOW_ERRPIN);
    } else {
        GPIO_setOutputLowOnPin(STATUSPORT, FLOW_ERRPIN);
    }

    // LED indicating if there is a sensing error
    if (state & SENSE_ERR) {
        GPIO_setOutputHighOnPin(STATUSPORT, SENSE_ERRPIN);
    } else {
        GPIO_setOutputLowOnPin(STATUSPORT, SENSE_ERRPIN);
    }
}
