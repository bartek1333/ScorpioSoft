/*
 * gps.c
 *
 *  Created on: 22 lis 2017
 *      Author: bartek
 */


#include "gps.h"
#include "stm32f4xx.h"
#include "main.h"
#include "stdint.h"
#include <stdlib.h>
#include <string.h>


#define GPS_BUF_PARSE_SIZE 13

// bufor odbiorczy z gps
uint8_t buffRx[GPS_BUFOR_RX_SIZE];
// bufor do parsowania
uint8_t buffRxParse[GPS_BUFOR_RX_SIZE];
// ilosc znakow w buforze
int buffRxInd;

sGpsData gpsData;

// test
uint8_t test_parse3, test_parse4, test_parse5, test_parse0;

void GPS_Init()
{
	memset(buffRx, 0, GPS_BUFOR_RX_SIZE);
	buffRxInd = 0;
	GPS_ResetData();
}


void My_UART4_Handler(uint8_t rec)
{
	if(buffRxInd == GPS_BUFOR_RX_SIZE)
	{
		buffRxInd = 0;
	}

	if(rec == '\n')
	{
		int i;
		buffRxInd = 0;
		for(i = 0; i < GPS_BUFOR_RX_SIZE; i++)
		{
			buffRxParse[i] = buffRx[i];
		}
		GPS_ProcessNewData();
	}
	else if(rec == '$')
	{
		buffRxInd = 0;
		buffRx[buffRxInd++] = '$';
	}
	else
	{
		buffRx[buffRxInd++] = rec;
	}
}


bool GPS_Checksum()
{
	int poz_gw = 0;
	int i;
	const char inttohex_big[] = "0123456789ABCDEF";
	const char inttohex_small[] = "0123456789abcdef";
	uint8_t tmp;
	int my_checksum = 0;
	int gps_checksum = 0;//&f*
	// szukam sumy w ciagu, zeby go porownac
	// sume zawsze poprzedza gwiazdka '*'
	while(buffRxParse[poz_gw++] != '*')
	{
		// nie znaleziono gwiazdki, jakis blad
		if(poz_gw == GPS_BUFOR_RX_SIZE)
			return false;
	}
	// poz_gw wskazuje na pierwszy znak po gwiazdce
	// w liczeniu checksumy, nie bierze sie pod uwage gwiazdki oraz $
	for(i = 1; i < poz_gw-1; i++)
	{
		my_checksum ^= buffRxParse[i];
	}
	// poz_gw wskazuje na miejsce znaku za gwiazdka
	// 2 znaki za gwiazdka, sa heksadecymalne
	tmp = buffRxParse[poz_gw++];
	// sprawdz czy to jest hex
	if(!((tmp >= '0' && tmp < '9')
			|| (tmp >= 'A' && tmp <= 'F')
			|| (tmp >= 'a' && tmp <= 'f')))
	{
		return false;
	}
	for(i = 0; i < 16; i++)
	{
		if(tmp == inttohex_big[i] || tmp == inttohex_small[i])
			break;
	}
	gps_checksum = 16 * i;

	tmp = buffRxParse[poz_gw++];
	// sprawdz czy to jest hex
	if(!((tmp >= '0' && tmp < '9')
			|| (tmp >= 'A' && tmp <= 'F')
			|| (tmp >= 'a' && tmp <= 'f')))
	{
		return false;
	}
	for(i = 0; i < 16; i++)
	{
		if(tmp == inttohex_big[i] || tmp == inttohex_small[i])
			break;
	}
	gps_checksum += i;

	if(gps_checksum == my_checksum)
	{
		return true;
	}
	else
	{
		return false;
	}
}


void GPS_Parse()
{
	char buf[GPS_BUF_PARSE_SIZE];
	int buf_idx;

	memset(buf, 0, GPS_BUF_PARSE_SIZE);

	// dane zaczynaja sie od indeksu 7
	int idx = 7;

	// parse hours
	uint8_t tmp;
	tmp = (buffRxParse[idx++] - '0') * 10;
	tmp += (buffRxParse[idx++] - '0');
	gpsData.utc_h = tmp;

	// parse minutes
	tmp = (buffRxParse[idx++] - '0') * 10;
	tmp += (buffRxParse[idx++] - '0');
	gpsData.utc_m = tmp;

	// parse seconds
	buf_idx = 0;
	// (GPS_BUF_PARSE_SIZE - 2) because in the end must be 0
	while(buffRxParse[idx] != ',' && buf_idx != (GPS_BUF_PARSE_SIZE - 2))
	{
		buf[buf_idx++] = buffRxParse[idx++];
	}
	// so far, decimal part is ommited
	gpsData.utc_s = (uint8_t)atof(buf);

	memset(buf, 0, GPS_BUF_PARSE_SIZE);

	// parse szerokosc
	idx++; // char after colon
	buf_idx = 0;
	// (GPS_BUF_PARSE_SIZE - 2) because in the end must be 0
	while(buffRxParse[idx] != ',' && buf_idx != (GPS_BUF_PARSE_SIZE - 2))
	{
		buf[buf_idx++] = buffRxParse[idx++];
	}
	gpsData.szerokosc = (uint32_t)(atof(buf) * 10000.0);

	memset(buf, 0, GPS_BUF_PARSE_SIZE);

	// parse NS
	idx++;
	gpsData.szerokosc_NS = buffRxParse[idx++];

	// parse dlugosc
	idx++; // char after colon
	buf_idx = 0;
	// (GPS_BUF_PARSE_SIZE - 2) because in the end must be 0
	while(buffRxParse[idx] != ',' && buf_idx != (GPS_BUF_PARSE_SIZE - 2))
	{
		buf[buf_idx++] = buffRxParse[idx++];
	}
	gpsData.dlugosc = (uint32_t)(atof(buf) * 10000.0);

	memset(buf, 0, GPS_BUF_PARSE_SIZE);

	// parse WE
	idx++;
	gpsData.dlugosc_WE = buffRxParse[idx++];
}

bool GPS_CheckForGga()
{
	// szukam koncowki GGA na poczatku
	// $GPGGA
	if(buffRxParse[3] == 'G' && buffRxParse[4] == 'G' && buffRxParse[5] == 'A')
	{
		return true;
	}
	else
	{
		return false;
	}
}

void GPS_ProcessNewData()
{
	// czy jest to msg GGA
	if(GPS_CheckForGga())
	{
		// czy check suma jest ok
		if(GPS_Checksum())
		{
			// dane ok, parsowanie
			GPS_Parse();
		}
	}
}

void GPS_ResetData()
{
	gpsData.utc_h = 0;
	gpsData.utc_m = 0;
	gpsData.utc_s = 0;
	gpsData.szerokosc = 0;
	gpsData.szerokosc_NS = 0;
	gpsData.dlugosc = 0;
	gpsData.dlugosc_WE = 0;
}


