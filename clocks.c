#include "clocks.h"
#include "driverlib.h"

// this function initializes necessary clocks
// it also disables the watchdog timer
// this function should be the first thing called in main()
void setup_clocks (void) {
    // do not watch the dog
    WDT_A_hold(WDT_A_BASE);

    // set FLL to a reference freq of ~10 kHz (VLO)
    CS_initClockSignal(CS_FLLREF, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // set DCO to a frequency of ~160 kHz, 16x FLL freq
    CS_initFLL(160, 16);

    // set MCLK to use DCO
    // setting MCLK automatically sets SMCLK as well
    CS_initClockSignal(CS_MCLK, CS_DCOCLKDIV_SELECT, CS_CLOCK_DIVIDER_1);
    // set ACLK to use VLO, although I don't think it's currently used anywhere
    CS_initClockSignal(CS_ACLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // wait for oscillators to stabilize
    do {
        // clear fault flags
        CS_clearFaultFlag(CS_DCOFFG);
        // wait a bit to see if they reappear
        __delay_cycles(100);
    } while (CS_getFaultFlagStatus(CS_DCOFFG));
}
