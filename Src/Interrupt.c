#include "Interrupt.h"

/*
 * Switch for selection is connected to PA1
 * Switch for navigation is at PB0
 */

void Interrupt_INIT(void){

	//Enable GPIOA Clock
	RCC->AHB1ENR |= (1U << 0);

	//Enable GPIOA Clock
	RCC->AHB1ENR |= (1U << 1);

	//Select Input mode
	GPIOA->MODER &= ~( 3U << 2 ); //PA1 INPUT
	GPIOB->MODER &= ~( 3U << 0 ); //PB0 INPUT

	//Enable Pull-Up
	GPIOA->PUPDR |= ( 1U << 2 ); //PA1 Pull-up
	GPIOB->PUPDR &= ~(3U << 0);
	GPIOB->PUPDR |= ( 1U << 0 ); //PB0 Pull-up

	//Enable SYSCFG CLK
	RCC->APB2ENR |= ( 1U << 14 );

	//Select falling edge trigger
	EXTI->FTSR |= ( 1U << 1);
	EXTI->FTSR |= ( 1U << 0);

	//Port Code in SYSCFG_EXTICRx registers
	SYSCFG->EXTICR[0] &= ~(0xF << 4); //Port A is selected

	SYSCFG->EXTICR[0] &= ~(0xF << 0);
	SYSCFG->EXTICR[0] |= (0x1<< 0); //Port B is selected

	//IMR enable
	EXTI->IMR |= (1U << 0);
	EXTI->IMR |= (1U << 1);

	//Interrupt set enable
	NVIC->ISER[0] |= (1U << 6);
	NVIC->ISER[0] |= (1U << 7);

	//Configure Priority
	NVIC_SetPriority(EXTI0_IRQn,1);
	NVIC_SetPriority(EXTI1_IRQn,0);
}
