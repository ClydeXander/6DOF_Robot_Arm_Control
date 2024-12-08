#include "stm32f4xx.h"
#include "Delay.h"

static int myTick = 0;
void Delay_TIM_Config(void){

	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	
	TIM4->PSC = 0;
	TIM4->ARR = 42; //1MHz frequency
	TIM4->CCR1 |=TIM_CR1_URS;
	TIM4->DIER |=TIM_DIER_UIE;
	TIM4->EGR |=TIM_EGR_UG;
	
	NVIC_EnableIRQ(TIM4_IRQn);

}

void Delay_ms(int ms){

	TIM4->CR1 |=TIM_CR1_CEN;
	while(myTick < (ms*1000));
	TIM4->CR1 &=~ TIM_CR1_CEN;

}

void Delay_us(int us){

	TIM4->CR1 |=TIM_CR1_CEN;
	while(myTick < us);
	TIM4->CR1 &=~ TIM_CR1_CEN;

}


