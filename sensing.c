#include "sensing.h"
#include "adc.h"

#define FIELD_CAP_VALUE 450
#define WILT_POINT_VALUE 700
enum VWCStat check_vwc(void) {
    static uint16_t prev_vwc = 1024;
    enum VWCStat rtn;
    uint16_t vwc = adc_read_vwc();
    // note that the sensor voltage is inversely related to VWC
    if (vwc >= WILT_POINT_VALUE) {
        if (prev_vwc >= WILT_POINT_VALUE && prev_vwc <= vwc) {
            rtn = VWC_NOFLOW;
        } else {
            rtn = VWC_LOW;
        }
    } else if (vwc <= FIELD_CAP_VALUE) {
        rtn = VWC_HIGH;
    } else {
        rtn = VWC_OK;
    }
    prev_vwc = vwc;
    return rtn;
}

#define SOLAR_DAY_VALUE 500
uint8_t sense_daytime(void) {
    uint16_t voltage = adc_read_solar();

    if (voltage >= SOLAR_DAY_VALUE) {
        return 1;
    } else {
        return 0;
    }
}
