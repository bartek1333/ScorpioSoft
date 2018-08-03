/*
 * imu.h
 *
 *  Created on: 22 lis 2017
 *      Author: bartek
 */

#ifndef IMU_H_
#define IMU_H_

#include "stdint.h"
#include "gps.h"

#define ADRESS_REG_WHOAMI 0x0F

#define ADRESS_REG_OUT_X_A_L 0x28
#define ADRESS_REG_OUT_X_A_H 0x29
#define ADRESS_REG_OUT_Y_A_L 0x2A
#define ADRESS_REG_OUT_Y_A_H 0x2B
#define ADRESS_REG_OUT_Z_A_L 0x2C
#define ADRESS_REG_OUT_Z_A_H 0x2D
#define ADRESS_REG_CTRL_REG6_A 0x20
#define SET_ACC_ODR_119HZ 0x60

#define ADRESS_REG_OUT_X_G_L 0x18
#define ADRESS_REG_OUT_X_G_H 0x19
#define ADRESS_REG_OUT_Y_G_L 0x1A
#define ADRESS_REG_OUT_Y_G_H 0x1B
#define ADRESS_REG_OUT_Z_G_L 0x1C
#define ADRESS_REG_OUT_Z_G_H 0x1D
#define ADRESS_REG_CTRL_REG1_G 0x10
#define SET_GYRO_ODR_119HZ 0x60

#define ADRESS_REG_OUT_X_M_L 0x28
#define ADRESS_REG_OUT_X_M_H 0x29
#define ADRESS_REG_OUT_Y_M_L 0x2A
#define ADRESS_REG_OUT_Y_M_H 0x2B
#define ADRESS_REG_OUT_Z_M_L 0x2C
#define ADRESS_REG_OUT_Z_M_H 0x2D
#define ADRESS_REG_CTRL_REG1_M 0x20
#define ADRESS_REG_CTRL_REG2_M 0x21
#define ADRESS_REG_CTRL_REG3_M 0x22
#define ADRESS_REG_CTRL_REG4_M 0x23
#define ADRESS_REG_CTRL_REG5_M 0x24
#define SET_MAG_ODR_80 0x1C
#define SET_MAG_CONT_CONV_MODE 0x00 // need delete 2 LSBs
#define SET_ULTRA_HIGH_Z 0x0C


typedef struct ImuDataS
{
	int16_t acc_x;
	int16_t acc_y;
	int16_t acc_z;

	int16_t gyro_x;
	int16_t gyro_y;
	int16_t gyro_z;

	int16_t mag_x;
	int16_t mag_y;
	int16_t mag_z;

	int16_t azimuth;
}ImuData;

extern ImuData imuData;


void IMU_Init();
void IMU_GetAllData();
bool IMU_DataReadyAccGyro();
bool IMU_DataReadyMag();
uint8_t IMU_ReadFromRegisterAccGyro(uint8_t adress);
void IMU_WriteToRegisterAccGyro(uint8_t adress, uint8_t value);
uint8_t IMU_ReadFromRegisterMag(uint8_t adress);
void IMU_WriteToRegisterMag(uint8_t adress, uint8_t value);


#endif /* IMU_H_ */
