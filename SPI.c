#include "stm32f4xx.h"
#include "SPI.h"
#include "Delay.h"

void SPI_Config(void){

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	
	GPIOA->MODER |=GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER7_1;
	GPIOA->MODER &=~GPIO_MODER_MODER6;
	
	GPIOA->OTYPER &=~GPIO_OTYPER_OT4;
	GPIOA->OTYPER &=~GPIO_OTYPER_OT5;
	GPIOA->OTYPER &=~GPIO_OTYPER_OT7;
	GPIOA->PUPDR|=GPIO_PUPDR_PUPD6_0;
	
	GPIOA->OSPEEDR|=GPIO_OSPEEDR_OSPEED4_1 | GPIO_OSPEEDR_OSPEED5_1 | GPIO_OSPEEDR_OSPEED7_1;
	
	GPIOA->BSRR |=0x10;
	
	GPIOA->AFR[0] |= (0x06UL<<GPIO_AFRL_AFSEL5_Pos) | (0x06UL<<GPIO_AFRL_AFSEL7_Pos);
	
	SPI1->CR1 |= (0x01UL<<SPI_CR1_BR_Pos) | SPI_CR1_MSTR;
	SPI1->CR2 |=SPI_CR2_SSOE;
	SPI1->CR1 |=SPI_CR1_SPE;

}

void SPI_Write (uint8_t SPI_REG,uint32_t SPI_Data_Write){

	Delay_ms(50);
	GPIOA->BSRR |=(0x10<<16);
	SPI1->DR =SPI_REG;
	while(SPI1->SR & SPI_SR_BSY);
	while (SPI1->SR & SPI_SR_TXE);
	SPI1->DR =SPI_Data_Write;
	GPIOA->BSRR |=0x10;

}

uint32_t SPI_Receive (uint8_t SPI_REG){

	uint32_t SPI_Data_Receive;
	
	Delay_ms(50);
	GPIOA->BSRR |=(0x10<<16);
	SPI1->DR = SPI_REG;
	while(SPI1->SR & SPI_SR_BSY);
	while (SPI1->SR & SPI_SR_TXE);
	SPI_Data_Receive = SPI1->DR;
	GPIOA->BSRR |=0x10;
	
	return SPI_Data_Receive;

}

	


