################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/BME280.c \
../Core/Src/dmaEx.c \
../Core/Src/driverEx.c \
../Core/Src/freeRTOSQueueTest.c \
../Core/Src/freeRTOSSemaphoreTest.c \
../Core/Src/heap_4.c \
../Core/Src/i2c.c \
../Core/Src/interrupt.c \
../Core/Src/list.c \
../Core/Src/main.c \
../Core/Src/mpu6050.c \
../Core/Src/port.c \
../Core/Src/queue.c \
../Core/Src/spiEx.c \
../Core/Src/stm32g4xx_hal_msp.c \
../Core/Src/stm32g4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32g4xx.c \
../Core/Src/tasks.c \
../Core/Src/timers.c \
../Core/Src/uartEx.c 

OBJS += \
./Core/Src/BME280.o \
./Core/Src/dmaEx.o \
./Core/Src/driverEx.o \
./Core/Src/freeRTOSQueueTest.o \
./Core/Src/freeRTOSSemaphoreTest.o \
./Core/Src/heap_4.o \
./Core/Src/i2c.o \
./Core/Src/interrupt.o \
./Core/Src/list.o \
./Core/Src/main.o \
./Core/Src/mpu6050.o \
./Core/Src/port.o \
./Core/Src/queue.o \
./Core/Src/spiEx.o \
./Core/Src/stm32g4xx_hal_msp.o \
./Core/Src/stm32g4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32g4xx.o \
./Core/Src/tasks.o \
./Core/Src/timers.o \
./Core/Src/uartEx.o 

C_DEPS += \
./Core/Src/BME280.d \
./Core/Src/dmaEx.d \
./Core/Src/driverEx.d \
./Core/Src/freeRTOSQueueTest.d \
./Core/Src/freeRTOSSemaphoreTest.d \
./Core/Src/heap_4.d \
./Core/Src/i2c.d \
./Core/Src/interrupt.d \
./Core/Src/list.d \
./Core/Src/main.d \
./Core/Src/mpu6050.d \
./Core/Src/port.d \
./Core/Src/queue.d \
./Core/Src/spiEx.d \
./Core/Src/stm32g4xx_hal_msp.d \
./Core/Src/stm32g4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32g4xx.d \
./Core/Src/tasks.d \
./Core/Src/timers.d \
./Core/Src/uartEx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_NUCLEO_64 -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I"D:/stm32workspaces/blink/freeRTOSIncludes" -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/BSP/STM32G4xx_Nucleo -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/BME280.cyclo ./Core/Src/BME280.d ./Core/Src/BME280.o ./Core/Src/BME280.su ./Core/Src/dmaEx.cyclo ./Core/Src/dmaEx.d ./Core/Src/dmaEx.o ./Core/Src/dmaEx.su ./Core/Src/driverEx.cyclo ./Core/Src/driverEx.d ./Core/Src/driverEx.o ./Core/Src/driverEx.su ./Core/Src/freeRTOSQueueTest.cyclo ./Core/Src/freeRTOSQueueTest.d ./Core/Src/freeRTOSQueueTest.o ./Core/Src/freeRTOSQueueTest.su ./Core/Src/freeRTOSSemaphoreTest.cyclo ./Core/Src/freeRTOSSemaphoreTest.d ./Core/Src/freeRTOSSemaphoreTest.o ./Core/Src/freeRTOSSemaphoreTest.su ./Core/Src/heap_4.cyclo ./Core/Src/heap_4.d ./Core/Src/heap_4.o ./Core/Src/heap_4.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/interrupt.cyclo ./Core/Src/interrupt.d ./Core/Src/interrupt.o ./Core/Src/interrupt.su ./Core/Src/list.cyclo ./Core/Src/list.d ./Core/Src/list.o ./Core/Src/list.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mpu6050.cyclo ./Core/Src/mpu6050.d ./Core/Src/mpu6050.o ./Core/Src/mpu6050.su ./Core/Src/port.cyclo ./Core/Src/port.d ./Core/Src/port.o ./Core/Src/port.su ./Core/Src/queue.cyclo ./Core/Src/queue.d ./Core/Src/queue.o ./Core/Src/queue.su ./Core/Src/spiEx.cyclo ./Core/Src/spiEx.d ./Core/Src/spiEx.o ./Core/Src/spiEx.su ./Core/Src/stm32g4xx_hal_msp.cyclo ./Core/Src/stm32g4xx_hal_msp.d ./Core/Src/stm32g4xx_hal_msp.o ./Core/Src/stm32g4xx_hal_msp.su ./Core/Src/stm32g4xx_it.cyclo ./Core/Src/stm32g4xx_it.d ./Core/Src/stm32g4xx_it.o ./Core/Src/stm32g4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32g4xx.cyclo ./Core/Src/system_stm32g4xx.d ./Core/Src/system_stm32g4xx.o ./Core/Src/system_stm32g4xx.su ./Core/Src/tasks.cyclo ./Core/Src/tasks.d ./Core/Src/tasks.o ./Core/Src/tasks.su ./Core/Src/timers.cyclo ./Core/Src/timers.d ./Core/Src/timers.o ./Core/Src/timers.su ./Core/Src/uartEx.cyclo ./Core/Src/uartEx.d ./Core/Src/uartEx.o ./Core/Src/uartEx.su

.PHONY: clean-Core-2f-Src

