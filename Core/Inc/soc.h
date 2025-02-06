#ifndef INC_SOC_H_
#define INC_SOC_H_

#include <stdlib.h>

#include "main.h"

#define ADC_RESOLUTION 4096.0f  // 12-bit ADC
#define V_REF 3.3f              // Reference voltage (V)

#define MAX_SHUNT_AMPAGE 500000
#define MAX_SHUNT_VOLTAGE 2.62f

void SOC_getInitialCharge(batteryModule *batt);
void SOC_updateCharge(batteryModule *batt, uint32_t elapsed_time);

#endif
