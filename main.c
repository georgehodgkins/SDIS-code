#include "driverlib.h"
#include "globals.h"
#include "pins.h"
#include "adc.h"
#include "rtc.h"
#include "sensing.h"
#include "valve.h"
#include "status.h"
//#include "logging.h"
//#include "serial.h"

void main (void) {
    // set up clock system and disable watchdog
    // THIS MUST BE CALLED AS THE FIRST THING IN main()
    setup_clocks();

    // initialize globals
    init_globals();

    // initialize pins
    setup_pins();

    // initialize real-time clock
    setup_rtc();

    // set up analog to digital converter
    setup_adc();

    // configure SPI on eUSCI
    //setup_serial();

    /* Main control loop
     *
     * Enters low-power mode at the beginning of the loop,
     * and returns only when woken by various interrupts.
     * Interrupt handlers will set a flag and then return to
     * the main loop, which checks the flags and deals with them accordingly,
     * then loops back to enter LPM3 again until the next interrupt.
     */

    // state flags are managed in the main loop and correspond to status LEDs
    enum StateFlags state = NORMAL;
    close_valve();
    while (1) {

        // enter low power mode 3
        __low_power_mode_3();

        // RTC flag is set when the RTC times out
        if (interrupt_flags & RTC) {

            // Check VWC sensor
            enum VWCStat vwc_status = check_vwc(state);
            if (vwc_status == VWC_LOW) {
                // if VWC is low, queue a valve open
                // it will be opened once light is low
               state |= VQUEUED;
            } else if (vwc_status == VWC_HIGH) {
                // clear flow error indicator
                state &= ~FLOW_ERR;
                // if VWC is high, close valve if open
                if (state & VOPEN) {
                    close_valve();
                    state &= ~VOPEN;
                }
            } else if (vwc_status == VWC_NOFLOW) {
                // if the valve is open and no flow is detected, error
                if (state & VOPEN) {
                    state |= FLOW_ERR;
                }
            } else if (vwc_status == VWC_OK) {
                // clear flow error indicator
                state &= ~FLOW_ERR;
            }

            // check light sensor (solar panels)
            if (!sense_daytime()) {
                // if light is low (nighttime) and valve is queued, open valve
                if (state & VQUEUED) {
                    open_valve();
                    state &= ~VQUEUED;
                    state |= VOPEN;
                }
            }

            // clear RTC flag
            interrupt_flags &= ~RTC;
        }

        // POWER flag is set by SVS interrupts
        if (interrupt_flags & POWER) {
            // handle power issues (or at least log them)
        }

        // SERIAL flag is set when an SD card is inserted or removed
        if (interrupt_flags & SERIAL) {
            // write data to SD card
        }

        update_status_indicators(state);

        // for debug purposes
        // if this is removed, make sure to also remove the disable in the ISR below
        RTC_clearInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT_FLAG);
        RTC_enableInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);
    }
}


//-----Interrupt handlers-----
// These just set flags and then return to the main loop
#pragma vector=RTC_VECTOR
__interrupt void RTC_ISR(void) {
    interrupt_flags |= RTC;
    rtc_count++;
    RTC_disableInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);
    RTC_clearInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT_FLAG);
    __low_power_mode_off_on_exit();
}
