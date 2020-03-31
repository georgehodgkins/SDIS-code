#include "pins.h"
#include "driverlib.h"
#include <stdint.h>

void update_status_indicators (enum StateFlags state) {
    if (state & VOPEN) {
        GPIO_setOutputHighOnPin(STATUSPORT, VOPENPIN);
    } else {
        GPIO_setOutputLowOnPin(STATUSPORT, VOPENPIN);
    }

    if (state & VQUEUED) {
        GPIO_setOutputHighOnPin(STATUSPORT, VQUEUEDPIN);
    } else {
        GPIO_setOutputLowOnPin(STATUSPORT, VQUEUEDPIN);
    }

    if (state & FLOW_ERR) {
        GPIO_setOutputHighOnPin(STATUSPORT, FLOW_ERRPIN);
    } else {
        GPIO_setOutputLowOnPin(STATUSPORT, FLOW_ERRPIN);
    }

    if (state & SENSE_ERR) {
        GPIO_setOutputHighOnPin(STATUSPORT, SENSE_ERRPIN);
    } else {
        GPIO_setOutputLowOnPin(STATUSPORT, SENSE_ERRPIN);
    }
}
