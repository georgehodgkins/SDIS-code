// SD card interface using SPI
#include <stdint.h>
#include "serial.h"
#include "pins.h"
#include "driverlib.h"
#include "sdc.h"

// ready card to send/receive data
enum SDC_STAT sdc_init (void) {

    // response buffer
    uint8_t response[8];

    // command parameters (and CRC)
    uint8_t params[5] = {0};

    // enable eUSCI module (includes a wait for stabilization)
    serial_enable();

    // set CS high to put card in native mode
    sdc_setCS(1);
    // send dummy data for at least 74 clock cycles
    for (int i = 0; i < 80; ++i) {
        serial_send_byte(0xFF);
    }

    // send a software reset command
    params[4] = 0x95; // CRC, only required for initial command
    sdc_send_command_get_response(CMD_GO_IDLE_STATE, params, response, 1);

    // check that card is working
    if (response[0] != (uint8_t) R1_IN_IDLE_STATE) {
        return SDC_BAD_RESPONSE;
    }

    params[0] = (uint8_t) CMD_APP_SEND_OP_COND;
    params[4] = 0xFF; // CRC not needed
    sdc_send_command_get_response (CMD_APP_CMD, params, response, 1);

    // wait for idle state bit to be cleared
    while (response & R1_IN_IDLE_STATE) {
        serial_send_byte(0xFF);
    }

    // if response is not zero, an error occurred
    if (response) {
        return sdc_r1_to_stat(response);
    }

    // finally, we need to set the block size to 512 (for FAT fs)
    // 32-bit parameter argument
    &((uint32_t*)params) = 512;
    sdc_send_command_get_response(CMD_SET_BLOCKLEN, params, response, 1);

    return sdc_r1_to_stat(response);
}

// set the CS (chip select) line status
// local only
void sdc_setCS (uint8_t x) {
    if (x) {
        GPIO_setOutputHighOnPin(SPIPORT, SPICSPIN);
    } else {
        GPIO_setOutputLowOnPin(SPIPORT, SPICSPIN);
    }
}

// to be specific, wait for a valid non-dummy response
// local only
// TODO: fold this into send_command_get_response?
uint8_t sdc_wait_for_response (void) {
    uint8_t response;
    do {
        do {
            serial_send_byte(0xFF);
        } while (!serial_response_ready());
        response = serial_get_byte();
    } while (response & 0x80); // all valid responses have a zero MSB

    return response;
}

// send a command and wait for a valid response
// local only
void sdc_send_command_get_response (enum SDC_COMMAND cmd, uint8_t* params,
                          uint8_t* response, uint16_t response_len) {
    // drive CS low
    sdc_setCS(0);

    // send command and any parameters attached
    serial_send_byte((uint8_t) cmd);
    serial_send(params, 5);

    // send dummy data until a response is given
    response[0] = sdc_wait_for_response();

    // get the rest of the response if present
    for (int i = 1; i < response_len; ++i) {
        response[i] = serial_get_byte();
    }

    // drive CS high
    sdc_setCS(1);
}

// get the corresponding return status from the raw response
// local only
enum SDC_STAT sdc_r1_to_stat (enum SDC_R1_RESPONSE resp) {
    switch (resp) {
    case(R1_OK):
        return SDC_OK;
    case (R1_IN_IDLE_STATE):
        return SDC_CARD_IDLE;
    case(R1_BAD_CMD):
        return SDC_BAD_CMD;
    case(R1_BAD_CRD):
        return SDC_BAD_CRC;
    case(R1_BAD_PARAM):
    case(R1_BAD_ADDRESS):
        return SDC_BAD_PARAMS;
    default:
        return SDC_MISC_ERR;
    }
}
