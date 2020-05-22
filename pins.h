// this header contains pin mappings and the state flag enum definition
#pragma once
#include "driverlib.h"

// various state flags
enum StateFlags {NORMAL = 0x00, VOPEN = 0x01, VQUEUED = 0x02, FLOW_ERR = 0x04, SENSE_ERR = 0x08};

// analog input pins defined as GPIO pins, for GPIO functions
#define SENSINGPORT GPIO_PORT_P1
#define VWCPIN GPIO_PIN6
#define SOLARPIN GPIO_PIN7
// analog input pins defined as ADC pins, for ADC functionsSTA
#define VWCADCIN ADC_INPUT_A6
#define SOLARADCIN ADC_INPUT_A7

// SPI interface for SD card, defined as GPIO pins
#define SPIPORT GPIO_PORT_P1
// using P1SEL0 is compatible with legacy code which assumes one SEL bit per pin
// since SPI config is SEL = 0b01 i.e. P1SEL0 = 0b1
#define SPIPORTSEL P1SEL0
#define SPIPORTDIR P1DIR
#define SPIPORTIN P1IN
#define SPIPORTOUT P1OUT

#define SPICLKPIN GPIO_PIN1 // controlled by SPI controller
#define SPISIMOPIN GPIO_PIN2 // controlled by SPI controller
#define SPISOMIPIN GPIO_PIN3 // controlled by SPI controller

// chip select and card detect for SD card (used by library as GPIO)
#define SDCCSPORT GPIO_PORT_P1
#define SDCCSPORTOUT P1OUT
#define SDCCSPORTDIR P1DIR
#define SDCCSPIN GPIO_PIN0

// if card detect pin changes, the ISR defined below main() needs to change as well
#define SDCCDPORT GPIO_PORT_P2
#define SDCCDPORTIN P2IN
#define SDCCDPORTDIR P2DIR
#define SDCCDPIN GPIO_PIN7

// pins for valve actuation (digital outputs)
#define VALVEDRIVEPORT GPIO_PORT_P2
#define VALVEDRIVEPIN GPIO_PIN0

// pins for status LEDs (digital outputs)
#define STATUSPORT GPIO_PORT_P2
#define VOPENPIN GPIO_PIN1
#define VQUEUEDPIN GPIO_PIN2
#define FLOW_ERRPIN GPIO_PIN4
#define SENSE_ERRPIN GPIO_PIN5
#define XFER_STATPIN GPIO_PIN6

// function to configure pins based on the mappings above
void setup_pins(void);
