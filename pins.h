// this header contains pin mappings and the state flag enum definition
#pragma once
#include "driverlib.h"

// various state flags
enum StateFlags {NORMAL = 0x00, VOPEN = 0x01, VQUEUED = 0x02, FLOW_ERR = 0x04, SENSE_ERR = 0x08};

// analog input pins defined as GPIO pins, for GPIO functions
#define SENSINGPORT GPIO_PORT_P1
#define VWCPIN GPIO_PIN6
#define SOLARPIN GPIO_PIN7
// analog input pins defined as ADC pins, for ADC functions
#define VWCADCIN ADC_INPUT_A6
#define SOLARADCIN ADC_INPUT_A7

// SPI interface pins for SD card, defined as GPIO pins
#define SPIPORT GPIO_PORT_P1
#define SPICSPIN GPIO_PIN0 // digital output
#define SPICLKPIN GPIO_PIN1 // controlled by SPI controller
#define SPISIMOPIN GPIO_PIN2 // controlled by SPI controller
#define SPISOMIPIN GPIO_PIN3 // controlled by SPI controller

// pins for valve actuation (digital outputs)
#define VALVEDRIVEPORT GPIO_PORT_P2
#define VALVEDRIVEPIN GPIO_PIN0

// pins for status LEDs (digital outputs)
#define STATUSPORT GPIO_PORT_P2
#define VOPENPIN GPIO_PIN1
#define VQUEUEDPIN GPIO_PIN2
#define FLOW_ERRPIN GPIO_PIN4
#define SENSE_ERRPIN GPIO_PIN5

// function to configure pins based on the mappings above
void setup_pins(void);
