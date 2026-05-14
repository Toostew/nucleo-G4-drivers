/*
 * i2c.c
 *	implements I2C in bare metal for GY-521
 *  Created on: 13 May 2026
 *      Author: tooka
 */

//we will be using PB7 and PB8 for I2C1_SDA and SCL respectively

#define I2C1_BASE_ADDR		0x40005400
#define RCC_BASE_ADDR 		0x40021000


#define RCC_APB1ENR 		(*((volatile uint32_t *)(RCC_BASE_ADDR + 0x58UL)))	//for APB1 clock enable


#define I2C_CR1				(*((volatile uint32_t *)(I2C1_BASE_ADDR))) //CR1 has no offset
#define I2C_CR2				(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x04UL)))
#define I2C_TIMINGGR		(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x10UL)))
#define I2C_ISR				(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x18UL)))
#define I2C_ISR				(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x1CUL)))
#define I2C_RXDR			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x24UL)))
#define I2C_TXDR			(*((volatile uint32_t *)(I2C1_BASE_ADDR + 0x28UL)))


//configuration function for I2C
void I2C_Configuration(){

}


