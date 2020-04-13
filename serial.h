#pragma once
#include <stdint.h>

void setup_serial (void);

void serial_send_byte (uint8_t);

uint8_t serial_get_byte (void);

void serial_enable (void);

void serial_disable (void);
