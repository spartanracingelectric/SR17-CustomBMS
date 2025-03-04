#include "hv.h"
#include "adc.h"
#include "main.h"

void ReadHVInput(batteryModule *batt) {
	uint16_t adcValue;
	HAL_ADC_Start(&hadc1);
	if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
		adcValue = HAL_ADC_GetValue(&hadc1);
	}
    HAL_ADC_Stop(&hadc1);
    float voltage = ((float)adcValue / ADC_RESOLUTION) * V_REF;
    float amcOutput = voltage / GAIN_TLV9001;
    float hvInput = amcOutput * DIVIDER_RATIO;

    batt->pack_voltage = hvInput;
}

