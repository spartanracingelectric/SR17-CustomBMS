#include "balance.h"
#include "6811.h"
#include "can.h"
#include <stdio.h>
#include "usart.h"

//DEFAULT VALUES THAT ARE SET IN CONFIG REGISTERS
//static int GPIO[5] = { 1, 1, 1, 1, 1 };
//static int REFON = 0;
//static int DTEN = 1; (READ ONLY BIT, we dont change it)
//static int ADCOPT = 0;
//static uint8_t VUV = 0x00;
//static uint8_t VOV_and_VUV = 0x00;
//static uint8_t VOV = 0x00;
//static int DCTO[4] = { 1, 1, 1, 1 };
CAN_RxHeaderTypeDef rxHeader;
uint8_t rxData[8];
uint8_t balance = 0;			//FALSE
uint8_t balanceFinish = 0;

static uint8_t config[8][6] = { { 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 }, { 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 },
								{ 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 }, { 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 },
								{ 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 }, { 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 },
								{ 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 }, { 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 } };

static uint8_t defaultConfig[8][6] = {{ 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 }, { 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 },
									  { 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 }, { 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 },
									  { 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 }, { 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 },
									  { 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 }, { 0xF8, 0x00, 0x00, 0x00, 0x00, 0x20 } };

void Balance_init(uint16_t *balanceStatus){
	balance = 0;
	balanceFinish = 0;
	Balance_reset(balanceStatus);
	Wakeup_Sleep();
	LTC_writeCFG(NUM_DEVICES, defaultConfig);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan1) {
//    printf("fifo 0 callback\n");
    if (HAL_CAN_GetRxMessage(hcan1, CAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK) {
        if (rxHeader.StdId == 0x604) {  // CAN message from charger
            uint8_t balanceCommand = rxData[0]; // see the data bit on CAN

            // change the BALANCE flag to enable balance
            if (balanceCommand == 1) {
            	balance = 1;  // enable balance
//                printf("BALANCE enabled by CAN message.\n");
            } else if (balanceCommand == 0) {
            	balance = 0;  // disable balance
            	balanceFinish = 1;
//                printf("BALANCE disabled by CAN message.\n");
            }
        }
    }
}

void Start_Balance(uint16_t *readVolt, uint16_t lowest, uint16_t *balanceStatus) {
//	printf("balance enable is %d\n", balance);
	if(balance > 0){
		Discharge_Algo(readVolt, lowest , balanceStatus);
		Wakeup_Sleep();
		LTC_writeCFG(NUM_DEVICES, config);
	}
	else{
		return;
	}
}

void End_Balance(uint16_t *balanceStatus) {
	if(balanceFinish == 1){
		Balance_reset(balanceStatus);
		Wakeup_Sleep();
		LTC_writeCFG(NUM_DEVICES, defaultConfig);
		balanceFinish = 0;
	}
	else{
		return;
	}
}

/**
 * perform balance
 * 
 * @param readVolt array containing cells volts.
 * @param length count of readings. 
 * @param lowest readVolt's lowest cell reading
 */
void Discharge_Algo(uint16_t *readVolt, uint16_t lowest, uint16_t *balanceStatus) {
	for (uint8_t devIdx = 0; devIdx < NUM_DEVICES; devIdx++) {
		// check if each cell is close within 0.005V of the lowest cell.
		uint8_t DCC[12];
		for (uint8_t cellIdx = 0; cellIdx < NUM_CELL_SERIES_GROUP; cellIdx++) {
			if (readVolt[devIdx * NUM_CELL_SERIES_GROUP + cellIdx] - lowest > BALANCE_THRESHOLD) {
				DCC[cellIdx] = 1;
				balanceStatus[devIdx] |= (1 << cellIdx);
			} else {
				DCC[cellIdx] = 0;
				balanceStatus[devIdx] &= ~(1 << cellIdx); //set the bit to 0
			}
		}
		Set_Cfg(devIdx, (uint8_t*) DCC);
	}
}

void Balance_reset(uint16_t *balanceStatus) {
	uint8_t DCC[12] = {0};  //reset all DCC to 0
	for (uint8_t devIdx = 0; devIdx < NUM_DEVICES; devIdx++) {
		balanceStatus[devIdx] = 0;
//		printf("balanceStaus[%d]: %d\n", devIdx, balanceStatus[devIdx]);

		Set_Cfg(devIdx, (uint8_t*) DCC);
	}
}

/**
 * setting configuration registers
 *
 * @param device index
 * @param array of DCC bits
 */
void Set_Cfg(uint8_t devIdx, uint8_t *DCC) {
	for (uint8_t cellIdx = 0; cellIdx < NUM_CELL_SERIES_GROUP; cellIdx++) {
		if (DCC[cellIdx]) {
			if (cellIdx < 8) {
				config[devIdx][4] |= (1 << cellIdx);
			} else if (cellIdx >= 8) {
				config[devIdx][5] |= (1 << (cellIdx - 8));
			}
		} else {
			if (cellIdx < 8) {
				config[devIdx][4] &= (~(1 << cellIdx));
			} else {
				config[devIdx][5] &= (~(1 << (cellIdx - 8)));
			}
		}
	}
}

