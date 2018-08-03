/*
 * communication.h
 *
 *  Created on: 25 lut 2018
 *      Author: bartek
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include "can.h"


extern CanTxMsgTypeDef TxMsg;
extern CanRxMsgTypeDef RxMsg;
extern CAN_FilterConfTypeDef sFilterConfig;

void CAN_Init();
void My_CAN_Handler();
void CAN_SendSzerokoscGeo();
void CAN_SendDlugoscGeo();
void CAN_SendTime();
void CAN_SendData(uint8_t* data, uint8_t size);



#endif /* COMMUNICATION_H_ */
