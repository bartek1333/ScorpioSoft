/*
 * gps.h
 *
 *  Created on: 22 lis 2017
 *      Author: bartek
 */

#ifndef GPS_H_
#define GPS_H_

#include "stdint.h"


#define GPS_BUFOR_RX_SIZE 82


typedef enum
{
	false = 0,
	true = 1
}bool;


typedef struct
{
	uint8_t utc_h;
	uint8_t utc_m;
	// floating point of second is ommited
	uint8_t utc_s;

	// szerokosc do float, potem * 10000
	uint32_t szerokosc;
	uint8_t szerokosc_NS;

	// dlugosc do float, potem * 10000
	uint32_t dlugosc;
	uint8_t dlugosc_WE;
}sGpsData;


extern uint8_t buffRx[GPS_BUFOR_RX_SIZE];
extern uint8_t buffRxParse[GPS_BUFOR_RX_SIZE];
extern sGpsData gpsData;

void GPS_Init();
bool GPS_Checksum();
void GPS_Parse();
bool GPS_CheckForGga();
void GPS_ProcessNewData();
void GPS_ResetData();
void My_UART4_Handler(uint8_t);


#endif /* GPS_H_ */
