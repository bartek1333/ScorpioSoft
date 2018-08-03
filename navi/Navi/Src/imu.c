/*
 * imu.c
 *
 *  Created on: 22 lis 2017
 *      Author: bartek
 */


#include "imu.h"
#include "main.h"
#include "gpio.h"
#include "spi.h"
#include <math.h>

#define DECLINATION (-8.58)


ImuData imuData;

void IMU_Init()
{
	uint8_t ret;
	// zaden nieaktywny
	HAL_GPIO_WritePin(CS_AG_GPIO_Port, CS_AG_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CS_M_GPIO_Port, CS_M_Pin, GPIO_PIN_SET);

	HAL_Delay(50);
	  ret = IMU_ReadFromRegisterAccGyro(ADRESS_REG_WHOAMI);
	  if(ret == 0x68)
	  {
		  //HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
	  }
	  HAL_Delay(50);
	  ret = IMU_ReadFromRegisterMag(ADRESS_REG_WHOAMI);
	  if(ret == 0x3D)
	  {
		  //HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
	  }

	  // set up acc gyro
	IMU_WriteToRegisterAccGyro(ADRESS_REG_CTRL_REG6_A, SET_ACC_ODR_119HZ);
	IMU_WriteToRegisterAccGyro(ADRESS_REG_CTRL_REG1_G, SET_GYRO_ODR_119HZ);

	// set up mag
	IMU_WriteToRegisterMag(ADRESS_REG_CTRL_REG1_M, SET_MAG_ODR_80);
	IMU_WriteToRegisterMag(ADRESS_REG_CTRL_REG2_M, 0x00);
	IMU_WriteToRegisterMag(ADRESS_REG_CTRL_REG3_M, 0x00);
	IMU_WriteToRegisterMag(ADRESS_REG_CTRL_REG4_M, 0x00);
	IMU_WriteToRegisterMag(ADRESS_REG_CTRL_REG5_M, 0x00);
}

void IMU_GetAllData()
{
	uint16_t dataH, dataL;

	dataH = (uint16_t)IMU_ReadFromRegisterAccGyro(ADRESS_REG_OUT_X_A_H);
	dataL = (uint16_t)IMU_ReadFromRegisterAccGyro(ADRESS_REG_OUT_X_A_L);
	imuData.acc_x = (dataH << 8) | dataL;
	dataH = (uint16_t)IMU_ReadFromRegisterAccGyro(ADRESS_REG_OUT_Y_A_H);
	dataL = (uint16_t)IMU_ReadFromRegisterAccGyro(ADRESS_REG_OUT_Y_A_L);
	imuData.acc_y = (dataH << 8) | dataL;
	dataH = (uint16_t)IMU_ReadFromRegisterAccGyro(ADRESS_REG_OUT_Z_A_H);
	dataL = (uint16_t)IMU_ReadFromRegisterAccGyro(ADRESS_REG_OUT_Z_A_L);
	imuData.acc_z = (dataH << 8) | dataL;

	dataH = (uint16_t)IMU_ReadFromRegisterAccGyro(ADRESS_REG_OUT_X_G_H);
	dataL = (uint16_t)IMU_ReadFromRegisterAccGyro(ADRESS_REG_OUT_X_G_L);
	imuData.gyro_x = (dataH << 8) | dataL;
	dataH = (uint16_t)IMU_ReadFromRegisterAccGyro(ADRESS_REG_OUT_Y_G_H);
	dataL = (uint16_t)IMU_ReadFromRegisterAccGyro(ADRESS_REG_OUT_Y_G_L);
	imuData.gyro_y = (dataH << 8) | dataL;
	dataH = (uint16_t)IMU_ReadFromRegisterAccGyro(ADRESS_REG_OUT_Z_G_H);
	dataL = (uint16_t)IMU_ReadFromRegisterAccGyro(ADRESS_REG_OUT_Z_G_L);
	imuData.gyro_z = (dataH << 8) | dataL;

	dataH = (uint16_t)IMU_ReadFromRegisterMag(ADRESS_REG_OUT_X_M_H);
	dataL = (uint16_t)IMU_ReadFromRegisterMag(ADRESS_REG_OUT_X_M_L);
	imuData.mag_x = (dataH << 8) | dataL;

	// hard coded xD
	imuData.mag_x *= 2;
	imuData.mag_x -= 2000;
	// hard coded xD

	dataH = (uint16_t)IMU_ReadFromRegisterMag(ADRESS_REG_OUT_Y_M_H);
	dataL = (uint16_t)IMU_ReadFromRegisterMag(ADRESS_REG_OUT_Y_M_L);
	imuData.mag_y = (dataH << 8) | dataL;
	dataH = (uint16_t)IMU_ReadFromRegisterMag(ADRESS_REG_OUT_Z_M_H);
	dataL = (uint16_t)IMU_ReadFromRegisterMag(ADRESS_REG_OUT_Z_M_L);
	imuData.mag_z = (dataH << 8) | dataL;



	float my = imuData.mag_y;
	float mx = imuData.mag_x;
	float heading;

	if(my == 0)
	{
		heading = (mx < 0) ? M_PI : 0;
	}
	else
	{
		heading = atan2(mx, my);
	}

	heading -= DECLINATION * M_PI / 180.0;

	if(heading > M_PI)
	{
		heading -= (2 * M_PI);
	}
	else if(heading < -M_PI)
	{
		heading += (2 * M_PI);
	}
	else if(heading < 0)
	{
		heading += 2 * M_PI;
	}

	imuData.azimuth = (int16_t)((heading * 180.0) / M_PI);
}


bool IMU_DataReadyAccGyro()
{
	return HAL_GPIO_ReadPin(DEN_AG_GPIO_Port, DEN_AG_Pin) == GPIO_PIN_SET;
}


bool IMU_DataReadyMag()
{
	return HAL_GPIO_ReadPin(DRDY_M_GPIO_Port, DRDY_M_Pin) == GPIO_PIN_SET;
}


uint8_t IMU_ReadFromRegisterAccGyro(uint8_t adress)
{
	uint8_t buff = 0;
	// to READ, 0 bit = 1
	//uint8_t adress_RW = 1;//0x80;
	// adress is on bits 0...6
	uint8_t adress_RW = (0x80 | (adress & 0x3F));//(adress << 1);
	// odblokuj acc gyro
	HAL_GPIO_WritePin(CS_AG_GPIO_Port, CS_AG_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &adress_RW, sizeof(adress_RW), 100);
	HAL_SPI_Receive(&hspi1, &buff, sizeof(buff), 100);
	HAL_GPIO_WritePin(CS_AG_GPIO_Port, CS_AG_Pin, GPIO_PIN_SET);
	return buff;
}


void IMU_WriteToRegisterAccGyro(uint8_t adress, uint8_t value)
{
	// to READ, 0 bit = 1
	// adress is on bits 0...6
	uint8_t adress_RW = (adress & 0x3F);
	// odblokuj acc gyro
	HAL_GPIO_WritePin(CS_AG_GPIO_Port, CS_AG_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &adress_RW, sizeof(adress_RW), 100);
	HAL_SPI_Transmit(&hspi1, &value, sizeof(value), 100);
	HAL_GPIO_WritePin(CS_AG_GPIO_Port, CS_AG_Pin, GPIO_PIN_SET);
}


uint8_t IMU_ReadFromRegisterMag(uint8_t adress)
{
	uint8_t buff = 0;
	// to READ, 0 bit = 1
	// adress is on bits 0...6
	uint8_t adress_RW = (0x80 | (adress & 0x3F));//(adress << 1);
	// odblokuj mag
	HAL_GPIO_WritePin(CS_M_GPIO_Port, CS_M_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &adress_RW, sizeof(adress_RW), 100);
	HAL_SPI_Receive(&hspi1, &buff, sizeof(buff), 100);
	HAL_GPIO_WritePin(CS_M_GPIO_Port, CS_M_Pin, GPIO_PIN_SET);
	return buff;
}


void IMU_WriteToRegisterMag(uint8_t adress, uint8_t value)
{
	// to READ, 0 bit = 1
	// adress is on bits 0...6
	uint8_t adress_RW = (adress & 0x3F);
	// odblokuj mag
	HAL_GPIO_WritePin(CS_M_GPIO_Port, CS_M_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &adress_RW, sizeof(adress_RW), 100);
	HAL_SPI_Transmit(&hspi1, &value, sizeof(value), 100);
	HAL_GPIO_WritePin(CS_M_GPIO_Port, CS_M_Pin, GPIO_PIN_SET);
}

