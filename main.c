#include "stm32f4xx.h"
#include "Delay.h"
#include "RCC_SystemClock_Config.h"
#include "I2C.h"


void TIM4_IRQHandler(void);

static int myTick = 0;

int main(void)
{
 
    RCC_SystemClock_Config();	
    I2C_Config();
		Delay_TIM_Config();
		MPU6050_Init();
	
		
    I2C_Restart(0x00, 0x06);
     
    I2C_Write_Single( 0x80, 0xFE, 0X7A); 
    
    I2C_Delete_Single(0x80, 0x00, 0x10);
    
    Delay_us(510);
     
    
    // 1.5ms PWM will be given (1,5ms/20ms)*4096=307 LED0_OFF_H= 1  LED0_OFF_H= 33 
    I2C_Write_Single( 0x80, 0x09, 0x01);
    I2C_Write_Single( 0x80, 0x08, 0x32); 
		
		
		// 1.5ms PWM will be given (1,5ms/20ms)*4096=307 LED1_OFF_H= 1  LED1_OFF_H= 33 
    I2C_Write_Single( 0x80, 0x0D, 0x01);
    I2C_Write_Single( 0x80, 0x0C, 0x32);

		// 1.5ms PWM will be given (1,5ms/20ms)*4096=307 LED2_OFF_H= 1  LED2_OFF_H= 33 
    I2C_Write_Single( 0x80, 0x11, 0x01);
    I2C_Write_Single( 0x80, 0x10, 0x32);
		
		// 1.5ms PWM will be given (1,5ms/20ms)*4096=307 LED3_OFF_H= 1  LED3_OFF_H= 33 
    I2C_Write_Single( 0x80, 0x15, 0x01);
    I2C_Write_Single( 0x80, 0x14, 0x32);
		
		// 1.5ms PWM will be given (1,5ms/20ms)*4096=307 LED4_OFF_H= 1  LED4_OFF_H= 33 
    I2C_Write_Single( 0x80, 0x19, 0x01);
    I2C_Write_Single( 0x80, 0x18, 0x32);
		
		// 1.5ms PWM will be given (1,5ms/20ms)*4096=307 LED5_OFF_H= 1  LED5_OFF_H= 33 
    I2C_Write_Single( 0x80, 0x1D, 0x01);
    I2C_Write_Single( 0x80, 0x1C, 0x32);
		
		// 1.5ms PWM will be given (1,5ms/20ms)*4096=307 LED6_OFF_H= 1  LED6_OFF_H= 33 
    I2C_Write_Single( 0x80, 0x21, 0x01);
    I2C_Write_Single( 0x80, 0x20, 0x32);
		

	while(1){

	}
}

void TIM4_IRQHandler(void){

	TIM4->SR &=~ TIM_SR_UIF ;
	myTick++;
	
	
}



