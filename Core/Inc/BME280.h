/*
 * BME280.h
 *
 *  Created on: 25 Jun 2026
 *      Author: tooka
 */

#ifndef INC_BME280_H_
#define INC_BME280_H_

#include <stdint.h> //<filename> usually for standard library, in the compile path
#include <stdio.h>
#include "i2c.h" //


typedef struct {
	uint32_t pres_20b; //20 bit stored in 32 bits
	uint32_t temp_20b;
	uint16_t hum_16b; //16 bit exactly
} BME_Measurements;

//there are 18 calibration values that need to be captured
//some of them are 16bit (2 8-bit register consecutively)
typedef struct {
	uint16_t 	dig_T1; //0x88, short
	int16_t 	dig_T2; //short
	int16_t 	dig_T3; //short
	uint16_t 	dig_P1; //short
	int16_t 	dig_P2; //short
	int16_t 	dig_P3; //short
	int16_t 	dig_P4; //short
	int16_t 	dig_P5; //short
	int16_t 	dig_P6; //short
	int16_t 	dig_P7; //short
	int16_t 	dig_P8; //short
	int16_t 	dig_P9; //0x9,  short
} Compensation_Constants_High;

typedef struct {
	uint8_t 	dig_H1; //char
} Compensation_Constants_A1;

typedef struct {
	int16_t		dig_H2; //0xE1, short
	uint8_t		dig_H3; //char
	int16_t		dig_H4; //short
	int16_t		dig_H5; //short
	int8_t		dig_H6; //0xE7,  char
} Compensation_Constants_Low;




#endif /* INC_BME280_H_ */
