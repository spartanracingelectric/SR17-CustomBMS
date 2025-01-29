	#include "hv.h"
	#include "adc.h"
	#include "main.h"

	void ReadHVInput(uint32_t *read_volt_HV) {
		uint32_t adcValue = 0;

		// ADC開始
		HAL_ADC_Start(&hadc1);
		if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
			adcValue = HAL_ADC_GetValue(&hadc1);
		}
		HAL_ADC_Stop(&hadc1);

		// 電圧計算
		float adcVoltage = ((float)adcValue / ADC_RESOLUTION) * V_REF;
		float amcOutput = adcVoltage / GAIN_TLV9001;
		float hvInput = amcOutput * DIVIDER_RATIO;

		*read_volt_HV = (uint32_t)(hvInput);
	}
