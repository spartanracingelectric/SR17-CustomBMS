#ifndef INC_HV_H_
#define INC_HV_H_

#include "main.h"

#define ADC_RESOLUTION 4096 //12-bit ADC
#define V_REF 3.3  			//Reference voltage (V)
#define GAIN_TLV9001 1.91 	//TLV9001 gain
#define GAIN_AMC1100 8.2  	//AMC1100 fixed gain
//Resistor values for the voltage divider
#define R1 2039200  		//2.0392 MΩ
#define R2 1000     		//1 kΩ
#define DIVIDER_RATIO 1 + R1 / R2

float ReadHVInput(void);

void State_of_Charge(struct batteryModule *batt, uint8_t *fault,
		uint8_t *warnings);


#endif /* INC_HV_H_ */