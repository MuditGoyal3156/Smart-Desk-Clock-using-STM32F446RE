#include "Interrupt.h"

/*
 * Switch for selection is connected to PA1
 * Switch for navigation is at PB0
 */

void Interrupt_INIT(void){

	//Enable GPIOA Clock
	RCC->AHB1ENR |= (1U << 0);

	//Enable GPIOB Clock
	RCC->AHB1ENR |= (1U << 1);

	//Select Input mode
	GPIOA->MODER &= ~( 3U << 2 ); //PA1 INPUT
	GPIOB->MODER &= ~( 3U << 0 ); //PB0 INPUT
	GPIOB->MODER &= ~( 3U << 6 ); //PB3 INPUT
	GPIOB->MODER &= ~( 3U << 8 ); //PB4 INPUT
	GPIOB->MODER &= ~( 3U << 10 ); //PB5 INPUT
	GPIOB->MODER &= ~( 3U << 20 ); //PB10 INPUT

	//Enable Pull-Up
	GPIOA->PUPDR |= ( 1U << 2 ); //PA1 Pull-up
	GPIOB->PUPDR &= ~(3U << 0);
	GPIOB->PUPDR |= ( 1U << 0 ); //PB0 Pull-up

	GPIOB->PUPDR &= ~(3U << 6);
	GPIOB->PUPDR |= ( 1U << 6); //PB3 Pull-up

	GPIOB->PUPDR &= ~(3U << 8);
	GPIOB->PUPDR |= ( 1U << 8); //PB4 Pull-up

	GPIOB->PUPDR &= ~(3U << 10);
	GPIOB->PUPDR |= ( 1U << 10 ); //PB5 Pull-up

	GPIOB->PUPDR &= ~(3U << 20);
	GPIOB->PUPDR |= ( 1U << 20 ); //PB10 Pull-up

	//Enable SYSCFG CLK
	RCC->APB2ENR |= ( 1U << 14 );

	//Select falling edge trigger
	EXTI->FTSR |= ( 1U << 0);
	EXTI->FTSR |= ( 1U << 1);
	EXTI->FTSR |= ( 1U << 3);
	EXTI->FTSR |= ( 1U << 4);
	EXTI->FTSR |= ( 1U << 5);
	EXTI->FTSR |= ( 1U << 10);


	//Port Code in SYSCFG_EXTICRx registers
	SYSCFG->EXTICR[0] &= ~(0xF << 4); //Port A is selected

	SYSCFG->EXTICR[0] &= ~(0xF << 0);
	SYSCFG->EXTICR[0] |= (0x1<< 0); //Port B is selected

	SYSCFG->EXTICR[0] &= ~(0xF << 12);
	SYSCFG->EXTICR[0] |= (0x1<< 12); //Port B is selected

	SYSCFG->EXTICR[1] &= ~(0xF << 0);
	SYSCFG->EXTICR[1] |= (0x1<< 0); //Port B is selected

	SYSCFG->EXTICR[1] &= ~(0xF << 4);
	SYSCFG->EXTICR[1] |= (0x1<< 4); //Port B is selected

	SYSCFG->EXTICR[2] &= ~(0xF << 8);
	SYSCFG->EXTICR[2] |= (0x1<< 8); //Port B is selected

	//IMR enable
	EXTI->IMR |= (1U << 0);
	EXTI->IMR |= (1U << 1);
	EXTI->IMR |= (1U << 3);
	EXTI->IMR |= (1U << 4);
	EXTI->IMR |= (1U << 5);
	EXTI->IMR |= (1U << 10);

	//Interrupt set enable
	NVIC->ISER[0] |= (1U << 6);
	NVIC->ISER[0] |= (1U << 7);
	NVIC->ISER[0] |= (1U << 9);
	NVIC->ISER[0] |= (1U << 10);
	NVIC->ISER[0] |= (1U << 23);
	NVIC->ISER[1] |= (1U << 8);//EXTI10_15

	//Configure Priority
	NVIC_SetPriority(EXTI0_IRQn,2);
	NVIC_SetPriority(EXTI1_IRQn,1);
	NVIC_SetPriority(EXTI3_IRQn,3);
	NVIC_SetPriority(EXTI4_IRQn,4);
	NVIC_SetPriority(EXTI5_IRQn,5);
	NVIC_SetPriority(EXTI10_IRQn,15);
}
