#include "driverlib.h"
#include "pins.h"
// functions for adc

// this function initializes the ADC to do sample-and-hold on two different channels
void setup_adc (void) {
    // set ADC to be software-triggered, use undivided SMCLK
    ADC_init(ADC_BASE, ADC_SAMPLEHOLDSOURCE_SC, ADC_CLOCKSOURCE_SMCLK, ADC_CLOCKDIVIDER_1);

    ADC_setResolution(ADC_BASE,ADC_RESOLUTION_10BIT);
    // set sample timing (not super important for single sample-and-hold)
    ADC_setupSamplingTimer(ADC_BASE, ADC_CYCLEHOLD_16_CYCLES, ADC_MULTIPLESAMPLESDISABLE);

    // enable ADC block
    ADC_enable(ADC_BASE);
}

// code to sample ADC, assuming it is already correctly configured
uint16_t sample_adc (void) {
    // start conversion
    ADC_startConversion(ADC_BASE, ADC_SINGLECHANNEL);

    // wait for completion
    while (ADC_isBusy(ADC_BASE) == ADC_BUSY) {}

    // store and return result
    uint16_t result = ADC_getResults(ADC_BASE);
    return result;
}

uint16_t adc_read_vwc(void) {
    // configure ADC to sample A2 (P1.2) and use supply as reference voltage
    ADC_disableConversions(ADC_BASE,ADC_COMPLETECONVERSION);
    ADC_configureMemory(ADC_BASE, VWCADCIN, ADC_VREFPOS_AVCC, ADC_VREFNEG_AVSS);

    // read and return VWC value
    uint16_t vwc = sample_adc();
    return vwc;
}

uint16_t adc_read_solar(void) {
    // configure ADC to sample A3 (P1.3) and use supply as voltage reference
    ADC_disableConversions(ADC_BASE,ADC_COMPLETECONVERSION);
    ADC_configureMemory(ADC_BASE, SOLARADCIN, ADC_VREFPOS_AVCC, ADC_VREFNEG_AVSS);

    // read and return solar panel voltage
    uint16_t voltage = sample_adc();
    return voltage;
}
