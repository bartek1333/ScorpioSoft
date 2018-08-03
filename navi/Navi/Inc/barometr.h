/*
 * barometr.h
 *
 *  Created on: 11 lut 2018
 *      Author: bartek
 */

#ifndef BAROMETR_H_
#define BAROMETR_H_

#include "stdint.h"


#define BAR_ADRESS_WRITE 0xEE
#define BAR_ADRESS_READ 0xEF
#define BAR_COMMAND_READ_T 0x32


void BAR_StartConversion();
void BAR_ReadTemperature();

#endif /* BAROMETR_H_ */
