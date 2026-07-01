#include "ADC.h"
#define STM32F446xx
#include "stm32f4xx.h"

/*
 * DATA -> PA0
 */
void ADC_GPIO_INIT(void){
	//Enable clock
	RCC->AHB1ENR |= (1U << 0);

	//Select Analog mode
	GPIOA->MODER |= (3U << 0);
}

void ADC_INIT(void){
	//Configure GPIO
	ADC_GPIO_INIT();

	//Enable Clock(ADC1)
	RCC->APB2ENR |= ( 1U << 8);

	//Set resolution in CR1
	ADC1->CR1 &= ~(3U << 24);
	ADC1->CR1 |= (2U << 24);

	//Set conversion sequence start
	ADC1->SQR3 &= ~(0xF << 0);	//First 4 bits reset to 0

	//Set Conversion sequence length
	ADC1->SQR1 &= ~(0xF << 20);	//Bit 20-23 reset to 0

	// Set sampling time for channel 0 (PA0)
	ADC1->SMPR2 &= ~(7U << 0);
	ADC1->SMPR2 |=  (7U << 0);

	//Enable continuous conversion mode
	ADC1->CR2 |= (1U << 1);	//Bit 1

	//Enable ADC(CR2 bit 0)
	ADC1->CR2 |= (1U << 0);

}

void ADC_START(void){
	 ADC1->CR2 |= (1U << 30);
}

void ADC_STOP(void){
	ADC1->CR2 &= ~(1U << 30);
}


uint8_t ADC_value(void)
{
	//Wait for conversion to finish
	while(!(ADC1->SR & (0x2)));//Bit 1

	//Read Data register and return the value

	return (uint8_t)(ADC1->DR & 0xFF);
}
