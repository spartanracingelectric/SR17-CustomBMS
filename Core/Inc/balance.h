#ifndef INC_BALANCE_H_
#define INC_BALANCE_H_

#include "main.h"

#define BALANCE_THRESHOLD 50

void Balance_init(uint16_t *balanceStatus);
void Start_Balance(uint16_t *readVolt, uint16_t lowest, uint16_t *balanceStatus);
void End_Balance(uint16_t *balanceStatus);
void Discharge_Algo(uint16_t *readVolt, uint16_t lowest, uint16_t *balanceStatus);
void Balance_reset(uint16_t *balanceStatus);
void Set_Cfg(uint8_t devIdx, uint8_t *DCC);

#endif /* INC_BALANCE_H_ */
