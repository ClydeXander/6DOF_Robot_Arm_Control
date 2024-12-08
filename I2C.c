#include "stm32f4xx.h"
#include "I2C.h"
#include "Delay.h"

#define MPU6050_ADDR 0xD0

#define SMPLRT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0X1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0X41
#define GYRO_XOUT_H_REG 0x43
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0X75

static int16_t Accel_X_RAW =0;
static int16_t Accel_Y_RAW =0;
static int16_t Accel_Z_RAW =0;

static int16_t Gyro_X_RAW =0;
static int16_t Gyro_Y_RAW =0;
static int16_t Gyro_Z_RAW =0;

static double Ax;
static double Ay;
static double Az;
static double Gx;
static double Gy;
static double Gz;

static uint32_t temp = 0;

void I2C_Config(void){
	
		/*********STEPS FOLLOWED*********
	1. Enable the I2C Clock and GPIO Clock
	2. Configure I2C Pins for alternate funtion
	3. Reset I2C
	4. Program the peripherald input clock in I2C_CR2 Register in order to generate correct timing
	5. configure the clock control register
	6. Configure the rise time register
	7. Program the I2C_CR1 register to enable the peripheral
	*/

	  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN ;
    RCC->APB2ENR |=RCC_APB2ENR_SYSCFGEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	
    
    GPIOB->MODER |=GPIO_MODER_MODE8_1; //PIN8 is selected as alternate function
    GPIOB->MODER |=GPIO_MODER_MODE9_1; //PIN9 is selected as alternate function
    GPIOB->OTYPER |=GPIO_OTYPER_OT8;   //PIN8 is configured as open-drain
    GPIOB->OTYPER |=GPIO_OTYPER_OT9;   //PIN9 is configured as open-drain
    GPIOB->OSPEEDR |=GPIO_OSPEEDER_OSPEEDR8_1; //PIN8 is confiigured as 50 MHz (High speed)
    GPIOB->OSPEEDR |=GPIO_OSPEEDER_OSPEEDR9_1; //PIN9 is confiigured as 50 MHz (High speed)
    GPIOB->AFR[1] |=(0x04) << GPIO_AFRH_AFSEL8_Pos;
    GPIOB->AFR[1] |=(0x04) << GPIO_AFRH_AFSEL9_Pos;
	
		I2C1->CR1 |=I2C_CR1_SWRST;
    I2C1->CR1 &=~I2C_CR1_SWRST;
	
		/* to calculate CCR (Clock Control Register) CCR= [((I2C_Period)/2)/(APB1_Period)] 
    I2C preiod is 100KHz Ti2c= 1/100K = 10us
    APB1 Period = 1/42 = 23.8 ns
    CCR = [(5000ns)/(23.8ns)]= 210
    */
	
		I2C1->CCR |=210 ;
    I2C1->TRISE |=43;
    I2C1->CR1 |=I2C_CR1_PE;

}

void I2C_Write_Single(uint8_t device_address, uint8_t mem_address, uint8_t data)
{
    
    I2C1->CR1 |=I2C_CR1_ACK;
    I2C1->CR1 |= I2C_CR1_START; 
    while (!(I2C1->SR1 & I2C_SR1_SB)); 

    I2C1->DR = device_address   ; 
    while (!(I2C1->SR1 & I2C_SR1_ADDR)); 
    temp = I2C1->SR1 | I2C1->SR2; 

    while (!(I2C1->SR1 & I2C_SR1_TXE)); 
    I2C1->DR = mem_address ; 
    while (!(I2C1->SR1 & I2C_SR1_BTF));
    
    while (!(I2C1->SR1 & I2C_SR1_TXE)); 
    I2C1->DR = data ; 
    while (!(I2C1->SR1 & I2C_SR1_BTF));
    
    I2C1->CR1 |= I2C_CR1_STOP; //generate stop condition
    

}

void I2C_Delete_Single(uint8_t device_address, uint8_t mem_address, uint8_t data)
{
    
    I2C1->CR1 |=I2C_CR1_ACK;
    I2C1->CR1 |= I2C_CR1_START; 
    while (!(I2C1->SR1 & I2C_SR1_SB)); 

    I2C1->DR = device_address   ; 
    while (!(I2C1->SR1 & I2C_SR1_ADDR)); 
    temp = I2C1->SR1 | I2C1->SR2; 

    while (!(I2C1->SR1 & I2C_SR1_TXE)); 
    I2C1->DR = mem_address ; 
    while (!(I2C1->SR1 & I2C_SR1_BTF));
    
    while (!(I2C1->SR1 & I2C_SR1_TXE)); 
    I2C1->DR &=~ data ; 
    while (!(I2C1->SR1 & I2C_SR1_BTF));
    
    I2C1->CR1 |= I2C_CR1_STOP; 
    

}

void I2C_Restart(uint8_t device_address, uint8_t data){
    
    I2C1->CR1 |=I2C_CR1_ACK;
    I2C1->CR1 |= I2C_CR1_START; 
    while (!(I2C1->SR1 & I2C_SR1_SB)); 

    I2C1->DR = device_address; 
    while (!(I2C1->SR1 & I2C_SR1_ADDR)); 
    temp = I2C1->SR1 | I2C1->SR2; 

    while (!(I2C1->SR1 & I2C_SR1_TXE)); 
    I2C1->DR = data; 
    while (!(I2C1->SR1 & I2C_SR1_BTF));
    while (!(I2C1->SR1 & I2C_SR1_TXE)); 
    
    
    I2C1->CR1 |= I2C_CR1_STOP; 
    
}

void I2C_Receive (uint8_t device_address, uint8_t mem_address, uint32_t *buffer, uint8_t size){

	int remaining = size;
	
		I2C1->CR1 |=I2C_CR1_ACK;
    I2C1->CR1 |= I2C_CR1_START; 
    while (!(I2C1->SR1 & I2C_SR1_SB)); 
	
		I2C1->DR = device_address + 0x1  ; 
    while (!(I2C1->SR1 & I2C_SR1_ADDR)); 
    temp = I2C1->SR1 | I2C1->SR2; 
	
	if (size == 1)
	{
		I2C1->DR = mem_address;  
		while (!(I2C1->SR1 & (1<<1)));  
			
		I2C1->CR1 &= ~(1<<10);   // clear the ACK bit 
		temp = I2C1->SR1 | I2C1->SR2;  
		I2C1->CR1 |= (1<<9);  // Stop I2C

		while (!(I2C1->SR1 & (1<<6)));  
			
		buffer[size-remaining] = I2C1->DR; 
		
	}
		
	else 
	{  
		
		while (remaining>2)
		{
			
		I2C1->DR = mem_address;  
		while (!(I2C1->SR1 & (1<<1)));  
		
		temp = I2C1->SR1 | I2C1->SR2;
			
			while (!(I2C1->SR1 & (1<<6)));  
			
			buffer[size-remaining] = I2C1->DR;  			
			
			I2C1->CR1 |= 1<<10; 
			
			remaining--;
			mem_address+=0x01;
		}
		
		while (!(I2C1->SR1 & (1<<6)));  
		buffer[size-remaining] = I2C1->DR;

		I2C1->CR1 &= ~(1<<10);  

		I2C1->CR1 |= (1<<9); 
		
		remaining--;

		while (!(I2C1->SR1 & (1<<6)));  
		buffer[size-remaining] = I2C1->DR;  
	}	

}


void MPU6050_Init (void){

	uint32_t check;
	
	I2C_Receive (MPU6050_ADDR, WHO_AM_I_REG, &check,1);
	Delay_ms(5);
	
	if(check==104){
	
		I2C_Write_Single(MPU6050_ADDR, PWR_MGMT_1_REG, 0x80);
		Delay_ms(5);
		
		I2C_Delete_Single(MPU6050_ADDR, PWR_MGMT_1_REG, 0x40);
		Delay_ms(510);
		
		I2C_Write_Single(MPU6050_ADDR, ACCEL_CONFIG_REG,0x10);
		Delay_ms(5);
		
		I2C_Write_Single(MPU6050_ADDR, GYRO_CONFIG_REG,0x10);
		Delay_ms(5);
		
	}
	
}

void MPU6050_Read_Accel(void){
	
	uint32_t Accel_data[6];

	I2C_Receive (MPU6050_ADDR, ACCEL_XOUT_H_REG, Accel_data,6);
	Delay_ms(5);
	
	Accel_X_RAW = (int16_t)(Accel_data[0]<<8 | Accel_data[1]);
	Accel_Y_RAW = (int16_t)(Accel_data[2]<<8 | Accel_data[3]);
	Accel_Z_RAW = (int16_t)(Accel_data[4]<<8 | Accel_data[5]);
	
	Ax = Accel_X_RAW/4096.0;
	Ay = Accel_Y_RAW/4096.0;
	Az = Accel_Z_RAW/4096.0;
}

void MPU6050_Read_Gyro(void){
	
	uint32_t Gyro_data[6];

	I2C_Receive (MPU6050_ADDR, GYRO_XOUT_H_REG, Gyro_data,6);
	Delay_ms(5);
	
	Gyro_X_RAW = (int16_t)(Gyro_data[0]<<8 | Gyro_data[1]);
	Gyro_Y_RAW = (int16_t)(Gyro_data[2]<<8 | Gyro_data[3]);
	Gyro_Z_RAW = (int16_t)(Gyro_data[4]<<8 | Gyro_data[5]);
	
	Gx = Accel_X_RAW/32.8 ;
	Gy = Accel_Y_RAW/32.8 ;
	Gz = Accel_Z_RAW/32.8 ;
}

