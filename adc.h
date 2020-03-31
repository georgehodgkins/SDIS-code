// this header links ADC functions
#pragma once
#include <stdint.h>

// initializes ADC
void setup_adc(void);

// returns VWC reading
uint16_t adc_read_vwc (void);

// returns solar voltage reading
uint16_t adc_read_solar (void);
