// initialization function for pin mappings
#include "driverlib.h" // TI driver library
#include "pins.h"

void setup_pins (void) {
    // start by configuring all pins as digital outputs
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN_ALL8);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN_ALL8);

    // set up SPI for SD card
    // redundant with TI library initialization function
    // GPIO_setAsPeripheralModuleFunctionOutputPin(SPIPORT,
    //     SPICLKPIN | SPISIMOPIN | SPISOMIPIN, GPIO_PRIMARY_MODULE_FUNCTION);

    // set up rising edge interrupt on SD card detect pin
    GPIO_selectInterruptEdge(SDCCDPORT, SDCCDPIN, GPIO_LOW_TO_HIGH_TRANSITION);
    GPIO_clearInterrupt(SDCCDPORT, SDCCDPIN);
    GPIO_enableInterrupt(SDCCDPORT, SDCCDPIN);

    // set up analog input
    GPIO_setAsPeripheralModuleFunctionInputPin(SENSINGPORT,
          VWCPIN | SOLARPIN, GPIO_TERNARY_MODULE_FUNCTION);

    // This is necessary to unlock config changes for certain pins
    // we do not actually use LPM5
    PMM_unlockLPM5();
}
