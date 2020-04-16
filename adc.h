// This header contains code for low-level ADC interfaces
#pragma once
#include <stdint.h>

// initializes ADC
void setup_adc(void);

// returns voltage from VWC sensor
uint16_t adc_read_vwc (void);

// returns voltage from solar panels
uint16_t adc_read_solar (void);
