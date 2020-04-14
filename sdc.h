#pragma once

// SD card commands
// each command byte is 01b followed by a 6-bit index
// this is not all the commands, only the ones used here
enum SDC_COMMAND {
    CMD_GO_IDLE_STATE = 0x40,
    CMD_SEND_OP_COND = 0x41,
    CMD_APP_SEND_OP_COND = 0x69, // app cmd
    CMD_SEND_IF_COND = 0x48,
    CMD_SEND_CSD = 0x49,
    CMD_SEND_CID = 0x4A,
    CMD_STOP_XMIT = 0x4C,
    CMD_SET_BLOCKLEN = 0x50,
    CMD_READ_BLOCK = 0x51,
    CMD_READ_MULT = 0x52,
    CMD_SET_WR_ERASE_COUNT = 0x57, // app cmd
    CMD_WRITE_BLOCK = 0x58,
    CMD_WRITE_MULT = 0x59,
    CMD_APP_CMD = 0x77,
    CMD_READ_OCR = 0x7A
};


// SD card responses
// MSB always zero
enum SDC_R1_RESPONSE {
    R1_OK = 0x00,
    R1_IN_IDLE_STATE = 0x01,
    R1_ERASE_RESET = 0x02,
    R1_BAD_CMD = 0x04,
    R1_BAD_CRC = 0x08,
    R1_BAD_ERASE_SEQ = 0x10,
    R1_BAD_ADDRESS = 0x20,
    R1_BAD_PARAM = 0x40
};

// return statuses for header functions
enum SDC_STAT {
    SDC_OK, SDC_NO_RESPONSE, SDC_BAD_RESPONSE, SDC_BAD_CMD, SDC_BAD_CRC,
    SDC_CARD_IDLE, SDC_BAD_PARAMS, SDC_MISC_ERR
};


// functions
enum SDC_STAT sdc_init (void);
