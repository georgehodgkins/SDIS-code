#pragma once
#include "driverlib.h"

enum StateFlags {NORMAL = 0x00, VOPEN = 0x01, VQUEUED = 0x02, FLOW_ERR = 0x04, SENSE_ERR = 0x08};

// define analog input pins
#define SENSINGPORT GPIO_PORT_P1
#define VWCPIN GPIO_PIN6
#define SOLARPIN GPIO_PIN7
#define VWCADCIN ADC_INPUT_A6
#define SOLARADCIN ADC_INPUT_A7

// define SPI interface pins for SD card
#define SPIPORT GPIO_PORT_P1
#define SPICSPIN GPIO_PIN0
#define SPICLKPIN GPIO_PIN1
#define SPISIMOPIN GPIO_PIN2
#define SPISOMIPIN GPIO_PIN3

// define pins for valve
#define VALVEDRIVEPORT GPIO_PORT_P2
#define VALVEDRIVEPIN GPIO_PIN0

// define pins for status LEDs
#define STATUSPORT GPIO_PORT_P2
#define VOPENPIN GPIO_PIN1
#define VQUEUEDPIN GPIO_PIN2
#define FLOW_ERRPIN GPIO_PIN4
#define SENSE_ERRPIN GPIO_PIN5

void setup_pins(void);
