#ifndef INC_HV_H_
#define INC_HV_H_

#include "main.h"

#define ADC_RESOLUTION 4096.0f  // 12-bit ADC
#define V_REF 3.3f              // Reference voltage (V)
#define GAIN_TLV9001 1.58f      // TLV9001 gain
// Resistor values for the voltage divider
#define R1 1400060.0f  // 1.4 MΩ
#define R2 6810.0f     // 6 863kΩ
#define DIVIDER_RATIO R1 / R2

void ReadHVInput(batteryModule *batt);

#endif /* INC_HV_H_ */
