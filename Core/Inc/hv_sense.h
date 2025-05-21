#ifndef INC_HV_SENSE_H_
#define INC_HV_SENSE_H_

#include "main.h"

#define GAIN_TLV9001 1.58f 		//TLV9001 gain
//Resistor values for the voltage divider
#define R1 1400000.0f  			//1.4 MΩ
#define R2 6810.f     			//6 863kΩ
#define DIVIDER_RATIO R1 / R2

void ReadHVInput(batteryModule *batt);

void getSumPackVoltage(batteryModule *batt);

#endif /* INC_HV_SENSE_H_ */
