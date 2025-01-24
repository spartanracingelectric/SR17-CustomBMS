#include "hv.h"

#include "adc.h"
#include "main.h"

void ReadHVInput(uint32_t *read_volt_HV) {
    // ADC開始
    uint32_t adcValue =
        MX_ADC_GetValue(&hadc1, GPIO_VSENSE_ADC_CHANNEL, HAL_MAX_DELAY);

    // 電圧計算
    float adcVoltage = ((float)adcValue / ADC_RESOLUTION) * V_REF * 10000.0;
    //    uint32_t amcOutput = adcVoltage / GAIN_TLV9001;
    float amcInput = adcVoltage / GAIN_AMC1300;
    float hvInput = amcInput * DIVIDER_RATIO;

    *read_volt_HV = (uint32_t)hvInput;
}

void State_of_Charge(struct batteryModule *batt, float elapsed_time, int *fault,
                     int *warnings) {
    uint32_t adcValue =
        MX_ADC_GetValue(&hadc1, GPIO_SOC_ADC_CHANNEL, HAL_MAX_DELAY);
    float voltage = ((float)adcValue / ADC_RESOLUTION) * V_REF;
    float current = (voltage / MAX_SHUNT_VOLTAGE) * MAX_SHUNT_AMPAGE;
    batt->soc = (uint32_t)((float)batt->soc -
                           current * (1 / (NUM_DEVICES)) *
                               (elapsed_time / 3600));
}
