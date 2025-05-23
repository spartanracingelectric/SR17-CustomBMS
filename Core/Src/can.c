/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    can.c
 * @brief   This file provides code for the configuration
 *          of the CAN instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *	 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "can.h"
#include "usart.h"
#include "stdio.h"

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;
uint8_t canSkipFlag = 0;
/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 9;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_3TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = ENABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
  CAN_FilterTypeDef sFilterConfig;
	  sFilterConfig.FilterBank = 0;
	  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	  sFilterConfig.FilterIdHigh = 0x604 << 5;  // Recieve only ID 0x604
	  sFilterConfig.FilterIdLow = 0x0000;
	  sFilterConfig.FilterMaskIdHigh = 0xFFF << 5;  // only accept complete match
	  sFilterConfig.FilterMaskIdLow = 0x0000;
	  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	  sFilterConfig.FilterActivation = ENABLE;

  HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_CAN1_2();

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */
	  canSkipFlag = 0;
  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */
  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

// uint8_t CAN_TX_HALT = 1; //halt frag to send it to mailbox

HAL_StatusTypeDef CAN_Start() { return HAL_CAN_Start(&hcan1); }

HAL_StatusTypeDef CAN_Activate() {
    return HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

HAL_StatusTypeDef CAN_Send(CANMessage *ptr) {
    uint32_t canErrapsTime = HAL_GetTick();
    while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    	if(canSkipFlag == 1){
    		return HAL_TIMEOUT;
    	}
    	else if(HAL_GetTick() - canErrapsTime > 10){
    		canSkipFlag = 1;
    		return HAL_TIMEOUT;
    	}
    }
    uint8_t *dataPtr = NULL;

	 if(ptr->TxHeader.StdId >= CAN_ID_VOLTAGE &&  ptr->TxHeader.StdId < CAN_ID_VOLTAGE + (NUM_CELLS * 2 / CAN_BYTE_NUM)) {//(NUM_CELLS * 2 / CAN_BYTE_NUM is just a number of can message
	   dataPtr = (uint8_t *)ptr->voltageBuffer;
	 }
	 else if(ptr->TxHeader.StdId >= CAN_ID_THERMISTOR &&  ptr->TxHeader.StdId < CAN_ID_THERMISTOR + (NUM_THERM_TOTAL / CAN_BYTE_NUM + 3)) {//(NUM_THERM_TOTAL / CAN_BYTE_NUM + 3) is a number of the message, 3 is number of sensors
	   dataPtr = (uint8_t *)ptr->thermistorBuffer;
	 }
	 else if (ptr->TxHeader.StdId == CAN_ID_SUMMARY) {
			dataPtr = (uint8_t *)ptr->summaryBuffer;
	 }
	 else if (ptr->TxHeader.StdId == CAN_ID_SAFETY) {
		dataPtr = (uint8_t *)ptr->safetyBuffer;
	 }
	 else if (ptr->TxHeader.StdId == CAN_ID_SOC) {
		dataPtr = (uint8_t *)ptr->socBuffer;
	 }
	 else if (ptr->TxHeader.StdId == CAN_ID_BALANCE_STATUS || ptr->TxHeader.StdId == CAN_ID_BALANCE_STATUS + 1) {
		dataPtr = (uint8_t *)ptr->balanceStatus;
	}
	return HAL_CAN_AddTxMessage(&hcan1, &ptr->TxHeader, dataPtr, &ptr->TxMailbox);
}

void CAN_SettingsInit(CANMessage *ptr) {
    CAN_Start();
    CAN_Activate();
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
    ptr->TxHeader.IDE = CAN_ID_STD;
    ptr->TxHeader.StdId = 0x00;
    ptr->TxHeader.RTR = CAN_RTR_DATA;
    ptr->TxHeader.DLC = 8;
}

void Set_CAN_Id(CANMessage *ptr, uint32_t id) { ptr->TxHeader.StdId = id; }

void CAN_Send_Voltage(CANMessage *buffer, uint16_t *readVolt) {
	uint32_t CAN_ID = (uint32_t)CAN_ID_VOLTAGE;
    for (int i = 0; i < NUM_CELLS; i += 4) {  //pack every 4 cell group in 1 CAN message
        buffer->voltageBuffer[0] =  readVolt[  i  ]       & 0xFF; 			//To ensure the data type is uint8_t, use & 0xFF
		buffer->voltageBuffer[1] = (readVolt[  i  ] >> 8) & 0xFF;
		buffer->voltageBuffer[2] =  readVolt[i + 1]       & 0xFF;
		buffer->voltageBuffer[3] = (readVolt[i + 1] >> 8) & 0xFF;
		buffer->voltageBuffer[4] =  readVolt[i + 2]       & 0xFF;
		buffer->voltageBuffer[5] = (readVolt[i + 2] >> 8) & 0xFF;
		buffer->voltageBuffer[6] =  readVolt[i + 3]       & 0xFF;
		buffer->voltageBuffer[7] = (readVolt[i + 3] >> 8) & 0xFF;
//        printf("can id for voltage: %d\n", CAN_ID);

        Set_CAN_Id(buffer, CAN_ID);
        CAN_Send(buffer);
        CAN_ID++;
    }
}

void CAN_Send_Temperature(CANMessage *buffer, uint16_t *readTemp, uint16_t *pressure, uint16_t *atmosTemp, uint16_t *humidity, uint16_t *dewPoint) {
    uint32_t CAN_ID = (uint32_t)CAN_ID_THERMISTOR;

    for (int i = 0; i < NUM_THERM_TOTAL; i += 12) {
        Set_CAN_Id(buffer, CAN_ID);
        buffer->thermistorBuffer[0] = (uint8_t)(readTemp[  i  ] & 0xFF);
		buffer->thermistorBuffer[1] = (uint8_t)(readTemp[i + 1] & 0xFF);
		buffer->thermistorBuffer[2] = (uint8_t)(readTemp[i + 2] & 0xFF);
		buffer->thermistorBuffer[3] = (uint8_t)(readTemp[i + 3] & 0xFF);
		buffer->thermistorBuffer[4] = (uint8_t)(readTemp[i + 4] & 0xFF);
		buffer->thermistorBuffer[5] = (uint8_t)(readTemp[i + 5] & 0xFF);
		buffer->thermistorBuffer[6] = (uint8_t)(readTemp[i + 6] & 0xFF);
		buffer->thermistorBuffer[7] = (uint8_t)(readTemp[i + 7] & 0xFF);

//		printf("temp1 in 8 bits:%d\n", ptr->data[0]);
//		printf("temp2 in 8 bits:%d\n", ptr->data[1]);
//		printf("temp3 in 8 bits:%d\n", ptr->data[2]);
//		printf("temp4 in 8 bits:%d\n", ptr->data[3]);
//		printf("temp5 in 8 bits:%d\n", ptr->data[4]);
//		printf("temp6 in 8 bits:%d\n", ptr->data[5]);
//		printf("temp7 in 8 bits:%d\n", ptr->data[6]);
//		printf("temp8 in 8 bits:%d\n", ptr->data[7]);
//        printf("can id for temp1: %d\n", CAN_ID);
		CAN_Send(buffer);
		CAN_ID++;

		Set_CAN_Id(buffer, CAN_ID);

		buffer->thermistorBuffer[0] = (uint8_t)(readTemp [i +  8] & 0xFF);
		buffer->thermistorBuffer[1] = (uint8_t)(readTemp [i +  9] & 0xFF);
		buffer->thermistorBuffer[2] = (uint8_t)(readTemp [i + 10] & 0xFF);
		buffer->thermistorBuffer[3] = (uint8_t)(readTemp [i + 11] & 0xFF);
		buffer->thermistorBuffer[4] = (uint8_t)(pressure  [i / 12] & 0xFF);
		buffer->thermistorBuffer[5] = (uint8_t)(atmosTemp[i / 12] & 0xFF);
		buffer->thermistorBuffer[6] = (uint8_t)(humidity  [i / 12] & 0xFF);
		buffer->thermistorBuffer[7] = (uint8_t)(dewPoint [i / 12] & 0xFF);

//		printf("temp9 in 8 bits:%d\n", ptr->data[0]);
//		printf("temp10 in 8 bits:%d\n", ptr->data[1]);
//		printf("temp11 in 8 bits:%d\n", ptr->data[2]);
//		printf("temp12 in 8 bits:%d\n", ptr->data[3]);
//		printf("can id for temp2: %d\n", CAN_ID);
		CAN_Send(buffer);
		CAN_ID++;
//      printf("sending CAN");
		}
//	for(int i = 0; i < 96; i ++){
//		uint8_t eightbit = (uint8_t)(readTemp[i]);
//		uint16_t sixteenbit = readTemp[i];
//		printf("temp[%d] in 8 bits:%d\n", i, eightbit);
//		printf("temp[%d] in 16 bits:%d\n", i, sixteenbit);
//	}
}

void CAN_Send_Cell_Summary(CANMessage *buffer, batteryModule *batt) {
	uint32_t CAN_ID = (uint32_t)CAN_ID_SUMMARY;
	Set_CAN_Id(buffer, CAN_ID);
	buffer->summaryBuffer[0] =  batt->cellVoltHighest         & 0xFF;
	buffer->summaryBuffer[1] = (batt->cellVoltHighest >> 8)   & 0xFF;
	buffer->summaryBuffer[2] =  batt->cellVoltLowest          & 0xFF;
	buffer->summaryBuffer[3] = (batt->cellVoltLowest >> 8)    & 0xFF;
	buffer->summaryBuffer[4] = (uint8_t)batt->cellTempHighest & 0xFF;
	buffer->summaryBuffer[5] = (uint8_t)batt->cellTempLowest  & 0xFF;
//	printf("can id for summary: %d\n", CAN_ID);
//	ptr->data[6] =
//	ptr->data[7] =
	CAN_Send(buffer);
//	printf("Summary\n");
}

void CAN_Send_Safety_Checker(CANMessage *buffer, batteryModule *batt, uint8_t *faults, uint8_t *warnings) {
	batt->cellDifference = batt->cellVoltHighest - batt->cellVoltLowest;
	uint32_t CAN_ID = (uint32_t)CAN_ID_SAFETY;
	Set_CAN_Id(buffer, CAN_ID);
	buffer->safetyBuffer[0] = *warnings;
	buffer->safetyBuffer[1] = *faults;
	buffer->safetyBuffer[2] =  batt->cellDifference           & 0xFF;
	buffer->safetyBuffer[3] = (batt->cellDifference     >> 8) & 0xFF;
	buffer->safetyBuffer[4] =  batt->hvsensPackVoltage       & 0xFF;
	buffer->safetyBuffer[5] = (batt->hvsensPackVoltage >> 8) & 0xFF;
	buffer->safetyBuffer[6] =  batt->sumPackVoltage          & 0xFF;
	buffer->safetyBuffer[7] = (batt->sumPackVoltage    >> 8) & 0xFF;
//	printf("can id for safety: %d\n", CAN_ID);
	CAN_Send(buffer);
//	printf("Faults\n");
}

void CAN_Send_SOC(CANMessage *buffer, batteryModule *batt,
                  uint16_t maxCapacity) {
	uint32_t CAN_ID = (uint32_t)CAN_ID_SOC;
    uint8_t percent = (uint8_t)((float) batt->soc * 100 / (float) maxCapacity);
    Set_CAN_Id(buffer, CAN_ID);
	buffer->socBuffer[0] = batt->soc;
	buffer->socBuffer[1] = batt->soc >> 8;
    buffer->socBuffer[2] = percent;
    buffer->socBuffer[3] = batt->current & 0xFF;
    buffer->socBuffer[4] = (batt->current >> 8) & 0xFF;
    buffer->socBuffer[5] = (batt->current >> 16) & 0xFF;
    buffer->socBuffer[6] = (batt->current >> 24)& 0xFF;
//    printf("can id for soc: %d\n", CAN_ID);
    CAN_Send(buffer);
}

void CAN_Send_Balance_Status(CANMessage *buffer, uint16_t *balanceStatus){
	uint32_t CAN_ID = (uint32_t)CAN_ID_BALANCE_STATUS;
	Set_CAN_Id(buffer, CAN_ID);

	buffer->balanceStatus[0] =  balanceStatus[0]       & 0xFF;
	buffer->balanceStatus[1] = (balanceStatus[0] >> 8) & 0xFF;
	buffer->balanceStatus[2] =  balanceStatus[1]       & 0xFF;
	buffer->balanceStatus[3] = (balanceStatus[1] >> 8) & 0xFF;
	buffer->balanceStatus[4] =  balanceStatus[2]       & 0xFF;
	buffer->balanceStatus[5] = (balanceStatus[2] >> 8) & 0xFF;
	buffer->balanceStatus[6] =  balanceStatus[3]       & 0xFF;
	buffer->balanceStatus[7] = (balanceStatus[3] >> 8) & 0xFF;
//	printf("can id for balance1: %d\n", CAN_ID);
	CAN_Send(buffer);
	CAN_ID++;

	Set_CAN_Id(buffer, CAN_ID);
	buffer->balanceStatus[0] =  balanceStatus[4]       & 0xFF;
	buffer->balanceStatus[1] = (balanceStatus[4] >> 8) & 0xFF;
	buffer->balanceStatus[2] =  balanceStatus[5]       & 0xFF;
	buffer->balanceStatus[3] = (balanceStatus[5] >> 8) & 0xFF;
	buffer->balanceStatus[4] =  balanceStatus[6]       & 0xFF;
	buffer->balanceStatus[5] = (balanceStatus[6] >> 8) & 0xFF;
	buffer->balanceStatus[6] =  balanceStatus[7]       & 0xFF;
	buffer->balanceStatus[7] = (balanceStatus[7] >> 8) & 0xFF;
//	printf("can id for balance2: %d\n", CAN_ID);
	CAN_Send(buffer);
}

//void CAN_Send_Sensor(struct CANMessage *ptr, batteryModule *batt) {
//    uint16_t CAN_ID = 0x602;
//	Set_CAN_Id(ptr, CAN_ID);
//
//	for (int i = 0; i < NUM_DEVICES; ++i) {
//		ptr->data[0] = batt->pressure  [i];
//		ptr->data[1] = batt->pressure  [i] >> 8;
//		ptr->data[2] = batt->atmosTemp[i];
//		ptr->data[3] = batt->atmosTemp[i] >> 8;
//		ptr->data[4] = batt->humidity  [i];
//		ptr->data[5] = batt->humidity  [i] >> 8;
//		ptr->data[6] = batt->dewPoint [i];
//		ptr->data[7] = batt->dewPoint [i] >> 8;
//		CAN_Send(ptr);
//
//		CAN_ID++;
//		Set_CAN_Id(ptr, CAN_ID);
//	}
//}
/* USER CODE END 1 */
