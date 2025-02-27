#include "hv.h"
#include "adc.h"
#include "main.h"

void ReadHVInput(batteryModule *batt) {
    float amcOutput = adc1_ch15 / GAIN_TLV9001;
    float hvInput = amcOutput * DIVIDER_RATIO;

    batt->pack_voltage = hvInput;
}

