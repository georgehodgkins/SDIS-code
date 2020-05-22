#include "driverlib.h" // TI driver library
#include "globals.h" // definition & initialization of global variables
#include "pins.h" // pin mappings
#include "adc.h" // low-level ADC methods
#include "rtc.h" // real-time clock initialization (ISR is defined below main())
#include "sensing.h" // logic for converting VWC & solar voltages to irrigation decisions
#include "valve.h" // valve actuation
#include "status.h" // status LED interface
#include "clocks.h" // system clock setup
// logging/SD card not fully implemented yet
#include "logging.h"

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

    /* Main control loop
     *
     * Enters low-power mode at the beginning of the loop,
     * and returns only when woken by various interrupts.
     * Interrupt handlers will set a flag (enum defined in pins.h) and then return to
     * the main loop, which checks the flags and deals with them accordingly,
     * then loops back to enter LPM3 again until the next interrupt.
     */

    // state flags are managed in the main loop and correspond to status LEDs
    enum StateFlags state = NORMAL;
    close_valve();
    while (1) {

        // enter low power mode 3
        __low_power_mode_3();

        // interrupts will return here (if uC is asleep when they trigger)

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
        if (interrupt_flags & CARDINSERT) {
            dump_logstore();
        }

        update_status_indicators(state);

        // here for debug purposes
        // if this is removed, make sure to also remove the disable in the ISR below
        RTC_clearInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT_FLAG);
        RTC_enableInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);
    }
}


//-----Interrupt handlers-----
// These just wake up, set flags, and then return to the main loop

// RTC interrupt
#pragma vector=RTC_VECTOR
__interrupt void RTC_ISR(void) {
    interrupt_flags |= RTC;
    rtc_count++;
    // interrupt is disabled here because the RTC period is set very short for debugging
    // and it's unhelpful to have the interrupt triggered before you get to the end of main()
    RTC_disableInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);
    RTC_clearInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT_FLAG);
    __low_power_mode_off_on_exit(); // do not go back to sleep
}

// SD card detection interrupt
#pragma vector=PORT2_VECTOR
__interrupt void SDC_ISR(void) {
    // this check is redundant for now since this is the only interrupt-enabled pin on P2
    if (GPIO_getInterruptStatus(SDCCDPORT, SDCCDPIN)) {
        interrupt_flags |= CARDINSERT;
        GPIO_clearInterrupt(SDCCDPORT, SDCCDPIN);
    }
    __low_power_mode_off_on_exit(); // do not go back to sleep
}

// Timer interrupt
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TA0_ISR (void) {
    GPIO_toggleOutputOnPin(STATUSPORT, XFER_STATPIN);
    Timer_A_clearTimerInterrupt(TA0_BASE);
}
