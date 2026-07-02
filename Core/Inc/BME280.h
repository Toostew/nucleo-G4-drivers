/*
 * BME280.h
 *
 *  Created on: 25 Jun 2026
 *      Author: tooka
 */

#ifndef INC_BME280_H_
#define INC_BME280_H_

#define hum_lsb		0xFE
#define hum_msb		0xFD
#define temp_xlsb	0xFC
#define temp_lsb	0xFB
#define temp_msb 	0xFA
#define press_xlsb	0xF9
#define press_lsb 	0xF8
#define press_msb	0xF7
#define config		0xF5
#define ctrl_meas	0xF4
#define status		0xF3
#define ctrl_hum	0xF2
#define reset		0xE0
#define id			0xD0



#include <stdint.h> //<filename> usually for standard library, in the compile path
#include <stdio.h>
#include "i2c.h" //
#include "dmaEx.h"
#include "uartEx.h"
#include "main.h"

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
	int16_t 	dig_P9; //0x9F,  short
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

typedef struct {
	Compensation_Constants_High high;
	Compensation_Constants_A1 a1;
	Compensation_Constants_Low low;
} Compensation_Constants;


void BME_I2C_Setup();



#endif /* INC_BME280_H_ */
