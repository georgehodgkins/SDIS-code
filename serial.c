// low-level serial functions
#pragma once
#include "driverlib.h"
#include "serial.h"

void setup_serial (void) {

    // define parameters for initialization
    struct EUSCI_A_SPI_initMasterParam spi_params {
        // source from SMCLK (~160 MHz, 16x VLO) with no divider
        .selectClockSource = EUSCI_A_CLOCKSOURCE_SMCLK,
        .clockSourceFrequency = CS_getSMCLK(),
        .desiredSpiClock = CS_getSMCLK(),
        // I think this is correct, but it might not be -- spec is very dense
        .msbFirst = EUSCI_A_SPI_MSB_FIRST,
        // CPHA = 0
        .clockPhase = EUSCI_A_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT, // this macro name is too long
        // CPOL = 0
        .clockPolarity = EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
        // 3 pin mode
        .spiMode = EUSCI_A_SPI_3PIN
    };

    // initialize with those parameters
    EUSCI_A_initMaster(EUSCI_A0_BASE, spi_params);
}

void serial_send_byte (uint8_t data) {
    while (!serial_transmit_ready()) {}
    EUSCI_A_SPI_transmitData (EUSCI_A0_BASE, data);
}

void serial_send (uint8_t* data_pt, uint16_t len) {
    for (uint16_t i = 0, i < len, ++i) {
        serial_send_byte(data_pt[i]);
    }
}

uint8_t serial_transmit_ready (void) {
    return EUSCI_A_SPI_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_TRANSMIT_INTERRUPT);
}

uint8_t serial_response_ready (void) {
    return EUSCI_A_SPI_getInterruptStatus(EUSCI_A0_BASE, EUSCI_A_RECEIVE_INTERRUPT)
}

uint8_t serial_get_byte (void) {
    // wait until response is ready
    while (!serial_response_ready()) {}
    return EUSCI_A_SPI_receiveData (EUSCI_A0_BASE);
}

void serial_enable (void) {
    EUSCI_A_SPI_enable(EUSCI_A0_BASE);
    __delay_cycles(1000);
}

void serial_disable (void) {
    EUSCI_A_SPI_disable(EUSCI_A0_BASE);
}
