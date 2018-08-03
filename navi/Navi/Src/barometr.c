/*
 * barometr.c
 *
 *  Created on: 11 lut 2018
 *      Author: bartek
 */


#include "barometr.h"
#include "i2c.h"

uint32_t temp;

void BAR_StartConversion()
{
	// OSR = 1024, temperature channel, 16.4 ms conversion time
	uint8_t adc_cvt = 0x4A;
	HAL_I2C_Master_Transmit(&hi2c2, BAR_ADRESS_WRITE, &adc_cvt, 1, 100);
}


void BAR_ReadTemperature()
{
	temp = 0;
	uint8_t ret[3];
	uint8_t adress = BAR_COMMAND_READ_T;
	HAL_I2C_Master_Transmit(&hi2c2, BAR_ADRESS_WRITE, &adress, 1, 100);
	HAL_I2C_Master_Receive(&hi2c2, BAR_ADRESS_READ, ret, 3, 100);
	// 4 MSB are useless
	ret[2] &= 0x0F;
	temp |= ret[2];
	temp <<= 16;
	temp |= ret[1];
	temp <<= 8;
	temp |= ret[0];
}
