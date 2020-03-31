#include "driverlib.h"
#include "rtc.h"
#pragma once

void setup_rtc(void) {
    // set up RTC to use a 10 kHz clock divided by 1024 (~9.76 Hz)
    // clock is selected by start function below as VLOCLK
    // period is .1024 sec, so 35157 cycles is about an hour
    RTC_init(RTC_BASE, 10, RTC_CLOCKPREDIVIDER_1024);

    // enable timeout interrupt on RTC
    RTC_clearInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT_FLAG);
    RTC_enableInterrupt(RTC_BASE, RTC_OVERFLOW_INTERRUPT);

    // start the RTC
    RTC_start(RTC_BASE, RTC_CLOCKSOURCE_VLOCLK);
}
