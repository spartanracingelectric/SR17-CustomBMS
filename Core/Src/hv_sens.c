#include <hv_sense.h>
#include "adc.h"
#include "main.h"
#include <stdio.h>
#include "usart.h"

	void ReadHVInput(batteryModule *batt) {
		uint32_t adcValue = 0;

		HAL_ADC_Start(&hadc1);//start adc with adc1
		if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
			adcValue = HAL_ADC_GetValue(&hadc1);
		}
		HAL_ADC_Stop(&hadc1);

		//calculate voltage based on  resolution and gain on opamp, voltage divider ratio
		float adcVoltage = ((float)adcValue / ADC_RESOLUTION) * 3.257; //3.258 is the vref
//		printf("adcVoltage for hv is: %f\n", adcVoltage);
		float amcOutput = adcVoltage / GAIN_TLV9001;
		float hvInput = (amcOutput) * (DIVIDER_RATIO) + 0.215; //- .6840 is offset

		batt->hvsens_pack_voltage = hvInput * 100;
	}

	void getSumPackVoltage(batteryModule *batt){
		uint32_t sum_voltage = 0;

		for (int i = 0; i < NUM_CELLS; i++) {
			 sum_voltage += batt->cell_volt[i]; //get sum voltage
		}
		batt->sum_pack_voltage = (uint16_t)(sum_voltage / 100);
	}


