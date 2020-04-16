// this file defines a function to initialize global variables
#include "globals.h"

void init_globals(void) {
    interrupt_flags = NONE;
    rtc_count = 0;
}
