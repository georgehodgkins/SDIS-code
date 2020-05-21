// function for real-time clock initialization
#include "driverlib.h"
#include "rtc.h"
#include "globals.h"
#include <stdint.h>
#pragma once

// initialize RTC and start it running with a timeout interrupt
void setup_rtc(void) {
    // set up RTC to use a 10 kHz clock divided by 1024 (~9.76 Hz)
    // clock is selected by start function below as VLOCLK
    // period is .1024 sec, so 35157 cycles is about an hour
    RTC_init(RTC_BASE, 10, RTC_CLOCKPREDIVIDER_1024);

    // enable timeout interrupt on RTC
    // the ISR is defined in main.c
    RTC_clearInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT_FLAG);
    RTC_enableInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);

    // start the RTC
    RTC_start(RTC_BASE, RTC_CLOCKSOURCE_VLOCLK);
}


// get current timestamp
uint32_t get_timestamp(void) {
    return (((uint32_t) rtc_count) << 16) | ((uint32_t) RTCCNT);
}
