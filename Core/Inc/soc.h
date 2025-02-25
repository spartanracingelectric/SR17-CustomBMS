#ifndef INC_SOC_H_
#define INC_SOC_H_

#include <stdlib.h>

#include "main.h"

#define MAX_SHUNT_AMPAGE 500000
#define MAX_SHUNT_VOLTAGE 2.68f

#define SHUNT_R1 33950.0f  // 1.4 MΩ
#define SHUNT_R2 28573.0f     // 6 863kΩ
#define SHUNT_DIVIDER_RATIO SHUNT_R1 / SHUNT_R2

void SOC_getInitialCharge(batteryModule *batt);
void SOC_updateCharge(batteryModule *batt, uint32_t elapsed_time);

#endif
