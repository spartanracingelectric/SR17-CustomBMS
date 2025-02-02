#ifndef INC_MODULE_H_
#define INC_MODULE_H_

#include "main.h"

#define MD_FAST 0b01		//27kHz
#define MD_NORMAL 0b10 		//7kHz
#define MD_FILTERED 0b11  	//26Hz
#define CELL_CH_ALL 0
#define DCP_DISABLED 0

void Read_Volt(uint16_t *read_volt);
void Get_Actual_Temps(uint8_t dev_idx, uint8_t tempindex, uint16_t *actual_temp,
		uint16_t data);
void Read_Temp(uint8_t tempindex, uint16_t *read_temp, uint16_t *read_auxreg);

#endif /* INC_MODULE_H_ */
