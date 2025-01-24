/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    adc.h
 * @brief   This file contains all the function prototypes for
 *          the adc.c file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define GPIO_SHUNT_ADC_CHANNEL 13
#define GPIO_VSENSE_ADC_CHANNEL 15

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

void MX_ADC1_Init(void);
void MX_ADC2_Init(void);

uint32_t MX_ADC_GetValue(ADC_HandleTypeDef *hadc, uint32_t channel,
                         uint32_t hal_delay) {
#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
