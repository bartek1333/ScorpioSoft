/*
 * communication.c
 *
 *  Created on: 25 lut 2018
 *      Author: bartek
 */


#include "communication.h"
#include "can.h"
#include "gps.h"
#include "imu.h"
#include <string.h>

CanTxMsgTypeDef TxMsg;
CanRxMsgTypeDef RxMsg;
CAN_FilterConfTypeDef sFilterConfig;


void CAN_Init()
{
	hcan1.pTxMsg = &TxMsg;
	hcan1.pRxMsg = &RxMsg;
	sFilterConfig.FilterNumber = 0;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh = 0x123 << 5;
	sFilterConfig.FilterIdLow = 0;
	sFilterConfig.FilterMaskIdHigh = 0;
	sFilterConfig.FilterMaskIdLow = 0;
	sFilterConfig.FilterFIFOAssignment = 0;
	sFilterConfig.BankNumber = 14;
	HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
}


void My_CAN_Handler()
{
	/*
	uint8_t rec_data = hcan1.pRxMsg->Data[0];
	switch(rec_data)
	{
	case 0x20:
		CAN_SendTime();
		break;

	}*/
	CAN_SendTime();
}


void CAN_SendTime()
{
	uint8_t buf[8];

	buf[0] = (uint8_t)(gpsData.dlugosc & 0xff);
	buf[1] = (uint8_t)((gpsData.dlugosc >> 8) & 0xff);
	buf[2] = (uint8_t)((gpsData.dlugosc >> 16) & 0xff);
	buf[3] = (uint8_t)((gpsData.dlugosc >> 24) & 0xff);
	buf[4] = (uint8_t)(gpsData.szerokosc & 0xff);
	buf[5] = (uint8_t)((gpsData.szerokosc >> 8) & 0xff);
	buf[6] = (uint8_t)((gpsData.szerokosc >> 16) & 0xff);
	buf[7] = (uint8_t)((gpsData.szerokosc >> 24) & 0xff);

	CAN_SendData(buf, 8);
}


void CAN_SendData(uint8_t* data, uint8_t size)
{
	hcan1.pTxMsg->StdId = 0x244;
	hcan1.pTxMsg->RTR = CAN_RTR_DATA;
	hcan1.pTxMsg->IDE = CAN_ID_STD;
	hcan1.pTxMsg->DLC = size;
	int i;
	for(i = 0; i < 8; i++)
	{
		hcan1.pTxMsg->Data[i] = data[i];
	}
	HAL_CAN_Transmit(&hcan1, 10);
}
