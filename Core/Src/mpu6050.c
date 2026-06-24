/*
 * mpu6050.c
 *
 *  Created on: 24 Jun 2026
 *      Author: tooka
 */


#include "mpu6050.h"
#include "i2c.h"

#define MPU_6050_GYRO_XH 		((uint8_t)(0x43))
#define MPU_6050_GYRO_XL		((uint8_t)(0x44))
#define MPU_6050_GYRO_YH		((uint8_t)(0x45))
#define MPU_6050_GYRO_YL		((uint8_t)(0x46))
#define MPU_6050_GYRO_ZH		((uint8_t)(0x47))
#define MPU_6050_GYRO_ZL		((uint8_t)(0x48))



Sensor_Data MPU_Measure_Gyro(){
	//there are 3 axis and we need to read 2 8-bit registers (each axis is stores a 16-bit value)
	//so 2 reads per axis, 3 axis so 6 reads

	int16_t tempTotal;
	uint8_t tempHigh;
	uint8_t tempLow;
	Sensor_Data Gyro_Measurements; //note that if we use pointers, we get using ->, otherwise, we use .

	//X data
	tempHigh = I2C_read(MPU_6050_GYRO_XH, 0);
	tempLow = I2C_read(MPU_6050_GYRO_XL, 0);
	tempTotal = ((tempHigh << 8) | (tempLow << 0));
	Gyro_Measurements.X_data = tempTotal;

	//Y data
	tempHigh = I2C_read(MPU_6050_GYRO_YH, 0);
	tempLow = I2C_read(MPU_6050_GYRO_YL, 0);
	tempTotal = ((tempHigh << 8) | (tempLow << 0));
	Gyro_Measurements.Y_data = tempTotal;

	//Y data
	tempHigh = I2C_read(MPU_6050_GYRO_ZH, 0);
	tempLow = I2C_read(MPU_6050_GYRO_ZL, 0);
	tempTotal = ((tempHigh << 8) | (tempLow << 0));
	Gyro_Measurements.Z_data = tempTotal;

	return Gyro_Measurements;

}
