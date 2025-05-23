#include "safety.h"
#include "main.h"
#include "stdio.h"
#include "gpio.h"
// ! Fault Thresholds

uint8_t highVoltFaultLock = 0;
uint8_t highVoltHysteresis = 0;
uint8_t lowVoltHysteresis = 0;
uint8_t lowVoltFaultLock = 0;
uint8_t cellImbalanceHysteresis = 0;
uint8_t highTempHysteresis = 0;

void Cell_Voltage_Fault(struct batteryModule *batt, uint8_t *fault, uint8_t *warnings){
//finding highest and lowest cell voltage
	batt->cellVoltHighest = batt->cellVolt[0];
	batt->cellVoltLowest = batt->cellVolt[0];

	for (int i = 0; i < NUM_CELLS; i++) {
//find highest volt
		if (batt->cellVolt[i] > batt->cellVoltHighest) {
			batt->cellVoltHighest = batt->cellVolt[i];
//			printf("high voltage fault: %d\n", batt->cellVoltHighest);
		}

//find lowest volt
		if (batt->cellVolt[i] < batt->cellVoltLowest) {
			batt->cellVoltLowest = batt->cellVolt[i];
		}
  }
//high cell volt warning
		if (batt->cellVoltHighest >= CELL_HIGH_VOLT_WARNING) {
			*warnings |= WARNING_BIT_HIGH_VOLT;
		}
//high cell volt fault
		if ((batt->cellVoltHighest >= CELL_HIGH_VOLT_FAULT)) {
			if (highVoltHysteresis > 2) {
				highVoltFaultLock = 1;
				*warnings &= ~WARNING_BIT_HIGH_VOLT;
				*fault |= FAULT_BIT_HIGH_VOLT;
				SendFaultSignal();
			} else {
				highVoltHysteresis++;
			}
//			printf("high voltage fault signal on\n");
		}
//reset high cell volt fault
		else if (batt->cellVoltHighest < (CELL_HIGH_VOLT_FAULT - FAULT_LOCK_MARGIN_HIGH_VOLT) && highVoltFaultLock == 1){
			if (highVoltHysteresis > 0){
				highVoltHysteresis = 0;
				highVoltFaultLock = 0;
				*warnings &= ~WARNING_BIT_HIGH_VOLT;
				*fault &= ~FAULT_BIT_HIGH_VOLT;
				ClearFaultSignal();
			}
		}

//low cell volt warning
		if (batt->cellVoltLowest <= CELL_LOW_VOLT_WARNING) {
			*warnings |= WARNING_BIT_LOW_VOLT;
		}
//low cell volt fault
		if (batt->cellVoltLowest <= CELL_LOW_VOLT_FAULT){
			if (lowVoltHysteresis > 2) {
				lowVoltFaultLock = 1;
				*warnings &= ~WARNING_BIT_LOW_VOLT;
				*fault |= FAULT_BIT_LOW_VOLT;
				SendFaultSignal();
			} else {
				lowVoltHysteresis++;
			}
//reset low cell volt fault
		} else if (batt->cellVoltLowest > (CELL_LOW_VOLT_FAULT + FAULT_LOCK_MARGIN_LOW_VOLT)) {
			if (lowVoltHysteresis > 0) {
				lowVoltHysteresis = 0;
				*warnings &= ~WARNING_BIT_LOW_VOLT;
				*fault &= ~FAULT_BIT_LOW_VOLT;
				ClearFaultSignal();
			}
		}
}
void Cell_Balance_Fault(struct batteryModule *batt, uint8_t *fault, uint8_t *warnings) {
	batt->cellDifference = batt->cellVoltHighest - batt->cellVoltLowest;
//cell volt imbalance warning
	if (batt->cellDifference >= CELL_VOLT_IMBALANCE_WARNING) {
		*warnings |= WARNING_BIT_IMBALANCE;
	}
}

void Cell_Temperature_Fault(struct batteryModule *batt, uint8_t *fault, uint8_t *warnings) {
	batt->cellTempHighest = batt->cellTemp[0];
	batt->cellTempLowest = batt->cellTemp[0];

	for (int i = 0; i < NUM_THERM_TOTAL; i++) {
		//find highest temp
		if (batt->cellTempHighest < batt->cellTemp[i]) {
			batt->cellTempHighest = batt->cellTemp[i];
		}
		if (batt->cellTempLowest > batt->cellTemp[i]) {
			batt->cellTempLowest = batt->cellTemp[i];
		}
	}

	//highest cell temp warning
	if (batt->cellTempHighest >= CELL_HIGH_TEMP_WARNING && batt->cellTempHighest < CELL_HIGH_TEMP_FAULT) {
		*warnings |= WARNING_BIT_HIGH_TEMP;
	}
	//highest cell temp fault

	if (batt->cellTempHighest >= CELL_HIGH_TEMP_FAULT) {
		if (highTempHysteresis > 2) {
			*warnings &= ~WARNING_BIT_HIGH_TEMP;
			*fault |= FAULT_BIT_HIGH_TEMP;
			SendFaultSignal();
		} else {
			(highTempHysteresis)++;
		}
	} else if (batt->cellTempHighest < (CELL_HIGH_TEMP_FAULT - FAULT_LOCK_MARGIN_HIGH_TEMP)) {
		if (highTempHysteresis > 0) {
			highTempHysteresis = 0;
			*warnings &= ~WARNING_BIT_HIGH_TEMP;
			*fault &= ~FAULT_BIT_HIGH_TEMP;
			ClearFaultSignal();
		}
	}
}

//void High_Voltage_Fault(struct batteryModule *batt, uint8_t *fault, uint8_t *warnings){
//	uint32_t sum_voltage = 0;
//
//	for (int i = 0; i < NUM_CELLS; i++) {
//		 sum_voltage += (uint32_t)batt->cellVolt[i]; //get sum voltage
//	}
//	if ((sum_voltage - batt->pack_voltage) >= FAULT_LOCK_MARGIN_LOW_VOLT){
//		*warnings |= WARNING_BIT_SLAVE_VOLT;
//	}
//	if (batt->pack_voltage >= PACK_HIGH_VOLT_WARNING) {
//		*warnings |= WARNING_BIT_HIGH_PACK_VOLT;
//	}
//	if (batt->pack_voltage <= PACK_LOW_VOLT_WARNING) {
//		*warnings |= WARNING_BIT_LOW_PACK_VOLT;
//	}
//	if (batt->pack_voltage >= PACK_HIGH_VOLT_FAULT) {
//		*fault |= FAULT_BIT_HIGH_PACK_VOLT;
//		HAL_GPIO_WritePin(MCU_SHUTDOWN_SIGNAL_GPIO_Port, MCU_SHUTDOWN_SIGNAL_Pin, GPIO_PIN_SET);
//	}
//	else{
//		*fault &= ~FAULT_BIT_HIGH_PACK_VOLT;
//	}
//	if (batt->pack_voltage <= PACK_LOW_VOLT_FAULT) {
//		*fault |= FAULT_BIT_LOW_PACK_VOLT;
//		HAL_GPIO_WritePin(MCU_SHUTDOWN_SIGNAL_GPIO_Port, MCU_SHUTDOWN_SIGNAL_Pin, GPIO_PIN_SET);
//	}
//	else{
//		*fault &= ~FAULT_BIT_LOW_PACK_VOLT;
//	}
//}


//void Module_Voltage_Averages(struct batteryModule *batt) {
//    for (int i = 0; i < NUM_CELLS; i += NUM_CELL_SERIES_GROUP) {
//        uint16_t volt_sum = 0;
//
//        for (int j = i; j < i + NUM_CELL_SERIES_GROUP && j < NUM_CELLS; j++) {
//            volt_sum += batt->cellVolt[j];
//        }
//
//        uint16_t average = volt_sum / NUM_CELL_SERIES_GROUP;
//
//        batt->average_volt[i / NUM_CELL_SERIES_GROUP] = average;
//    }
//}
//
//
//void Module_Temperature_Averages(struct batteryModule *batt) {
//    for (int i = 0; i < NUM_THERM_TOTAL; i += NUM_THERM_PER_MOD) {
//        uint16_t temp_sum = 0;
//
//        for (int j = i; j < i + NUM_THERM_PER_MOD && j < NUM_THERM_TOTAL; j++) {
//            temp_sum += batt->cellTemp[j];
//        }
//
//        uint16_t average = temp_sum / NUM_THERM_PER_MOD;
//
//        batt->average_temp[i / NUM_THERM_PER_MOD] = average;
//    }
//}
