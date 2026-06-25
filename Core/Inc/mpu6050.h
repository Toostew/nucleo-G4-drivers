/*
 * mpu6050.h
 *
 *  Created on: 24 Jun 2026
 *      Author: tooka
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#include <stdio.h>
#include <stdint.h>


//this is incomplete since it ONLY measures axis data for the gyro, there's still the accelorometer
typedef struct{
	int16_t X_data;
	int16_t Y_data;
	int16_t Z_data;
} Sensor_Data;
Sensor_Data MPU_Measure_Gyro();





#endif /* INC_MPU6050_H_ */
