/*
 * BME280.c
 *
 *  Created on: 25 Jun 2026
 *      Author: tooka
 */

#include "BME280.h"
#include "i2c.h"



//the BME280 is a sensor by Bosch to measure humidity, pressure and temperature
//unlike the MPU6050 this sensor needs quite a bit of pre-processing before we can dsplay the information
//the BME features register shadowing, wherein, when we begin reading the sensor data registers, the sensor stops updating the registers
//this is done to prevent innacurate data from accidental overwriting while reading (this is race condition!)
//the sensor also requires calibration with dedicated calibration registers, which you'll have to read too (just once at boot)
//in the BME280, burst reads automatically increment an internal pointer so you can read sequential registers faster


//we ought to capture
void BME_I2C_Setup(){

}




