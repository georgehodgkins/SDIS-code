// function to control status LEDs based on state flags
#include "pins.h"
#include "driverlib.h"
#include "status.h"
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

// set transfer indicator status
void set_xfer_LED (enum XferStatLED param) {
    Timer_A_stop(TA0_BASE);

    switch (param) {
    case XFER_LED_OFF:
    default:
        GPIO_setOutputLowOnPin(STATUSPORT, XFER_STATPIN);
        break;
    case XFER_LED_ON:
        GPIO_setOutputHighOnPin(STATUSPORT, XFER_STATPIN);
        break;
    case XFER_LED_BLINK:
        Timer_A_startCounter(TA0_BASE, TIMER_A_UP_MODE);
    }
}
