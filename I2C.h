#include "stdint.h"

void I2C_Write_Single(uint8_t device_address, uint8_t mem_address, uint8_t data);
void I2C_Restart(uint8_t device_address, uint8_t data);
void I2C_Delete_Single(uint8_t device_address, uint8_t mem_address, uint8_t data);
void I2C_Receive (uint8_t device_address,uint8_t Address, uint32_t *buffer, uint8_t size);
void I2C_Config(void);
void MPU6050_Read_Accel(void);
void MPU6050_Init (void);
void MPU6050_Read_Gyro(void);
