/*
 * RTC.c
 *
 *  Created on: 23-Jun-2026
 *      Author: mudit
 */
#include "RTC.h"

void RTC_INIT(RTC_TIME *time,RTC_DATE *date){
	//Enable PWR CLK(APB1)
	RCC->APB1ENR |= (1U << 28);

	// Set DBP bit of the PWR_CR
	PWR->CR |= (1U << 8);

	//Turn on LSE( LSEON bit in RCC_BDCR)
	RCC->BDCR |= (1U << 0);

	//Wait for Crystal to settle( LSERDY flag in RCC_BDCR)
	while(!(RCC->BDCR & (1U << 1)));

	//Select clock source RTCSEL[1:0] bits in RCC_BDCR
	RCC->BDCR &= ~(3U << 8);
	RCC->BDCR |= (1U << 8);

	//Enable RTC CLK
	RCC->BDCR |= (1U << 15);

	//Remove Write protection
	/*
	 * IN RTC_WPR Register
	 * Write 0xCA
	 * Write 0x53
	 * In order
	 */
	RTC->WPR = 0XCA;
	RTC->WPR = 0X53;

	//Set INIT bit to 1 in the RTC_ISR register to enter initialization mode
	RTC->ISR |= (1U << 7);

	//Poll INITF bit of in the RTC_ISR register for value 1
	while(!(RTC->ISR & (1U << 6)));

	//Program prescaler to generate a 1 Hz clock for the calendar counter
    RTC->PRER = (127 << 16) | 255;

	//Load the initial time and date values in the shadow registers (RTC_TR and RTC_DR)
    if (!(RTC->ISR & (1U << 4))) {  // INITS flag: 0 = not initialized
        INIT_DATE(date);
        INIT_TIME(time);
    }


	//Configure the time format (12 or 24 hours) through the FMT bit in the RTC_CR
    RTC->CR &= ~(1U << 6);

	//Exit the initialization mode by clearing the INIT bit
	RTC->ISR &= ~(1U << 7);

	//Enable Write Protection
	RTC->WPR = 0XFF;
}

void RTC_UPDATE(RTC_TIME *time,RTC_DATE *date){
	//Enable PWR CLK(APB1)
	RCC->APB1ENR |= (1U << 28);

	// Set DBP bit of the PWR_CR
	PWR->CR |= (1U << 8);

	//Turn on LSE( LSEON bit in RCC_BDCR)
	RCC->BDCR |= (1U << 0);

	//Wait for Crystal to settle( LSERDY flag in RCC_BDCR)
	while(!(RCC->BDCR & (1U << 1)));

	//Select clock source RTCSEL[1:0] bits in RCC_BDCR
	RCC->BDCR &= ~(3U << 8);
	RCC->BDCR |= (1U << 8);

	//Enable RTC CLK
	RCC->BDCR |= (1U << 15);

	//Remove Write protection
	/*
	 * IN RTC_WPR Register
	 * Write 0xCA
	 * Write 0x53
	 * In order
	 */
	RTC->WPR = 0XCA;
	RTC->WPR = 0X53;

	//Set INIT bit to 1 in the RTC_ISR register to enter initialization mode
	RTC->ISR |= (1U << 7);

	//Poll INITF bit of in the RTC_ISR register for value 1
	while(!(RTC->ISR & (1U << 6)));

	//Program prescaler to generate a 1 Hz clock for the calendar counter
    RTC->PRER = (127 << 16) | 255;

	//Load the initial time and date values in the shadow registers (RTC_TR and RTC_DR)
    INIT_DATE(date);
    INIT_TIME(time);



	//Configure the time format (12 or 24 hours) through the FMT bit in the RTC_CR
    RTC->CR &= ~(1U << 6);

	//Exit the initialization mode by clearing the INIT bit
	RTC->ISR &= ~(1U << 7);

	//Enable Write Protection
	RTC->WPR = 0XFF;
}
void Alarm_INIT(ALARM_TIME *time,ALARM_DATE *date){
	//Remove Write protection
	/*
	 * IN RTC_WPR Register
	 * Write 0xCA
	 * Write 0x53
	 * In order
	 */
	RTC->WPR = 0XCA;
	RTC->WPR = 0X53;

	//Disable ALARM
	RTC->CR &= ~(1U << 8);

	//POLL for write flag
	while(!(RTC->ISR & (1U << 0)));

	//Clear pending alarm flag
	RTC->ISR &= ~(1U << 8);
	EXTI->PR = (1U << 17);

	//Set alarm
	ALARM_DATE_AND_TIME(date,time);

	//Enable ALARM Interrupt
	RTC->CR |= (1U << 12);

	//Enable EXTI Line 17
	EXTI->IMR  |= (1U << 17);

	//Select rising edge trigger
	EXTI->RTSR |= (1U << 17);

	//Enable IRQ
	NVIC_SetPriority(RTC_Alarm_IRQn, 0);
	NVIC_EnableIRQ(RTC_Alarm_IRQn);

	//Enable ALARM
	RTC->CR |= (1U << 8);

	//Enable Write Protection
	RTC->WPR = 0xFF;
}
void ALARM_DATE_AND_TIME(ALARM_DATE *data,ALARM_TIME *time)
{
	uint8_t Hour = DecimaltoBCD(time->Hours);
	uint8_t Minute = DecimaltoBCD(time->Minutes);
	uint8_t Second = DecimaltoBCD(time->Seconds);

	uint32_t register_value = (Hour << 16) | (Minute << 8) | (Second << 0);

	// ALARM MASK CONFIGURATION
	register_value |=  (1U << 31);  // SET MSK4: Ignore the Date field (Daily Alarm)
	register_value &= ~(1U << 30);  // WDSEL: Date/WeekDay selection
	register_value &= ~(1U << 23);  // CLEAR MSK3: Match Hour exactly
	register_value &= ~(1U << 22);  // PM bit (Keep 0 for 24-hour format)
	register_value &= ~(1U << 15);  // CLEAR MSK2: Match Minute exactly
	register_value &= ~(1U << 7);   // CLEAR MSK1: Match Second exactly

	RTC->ALRMAR = register_value;
}



void INIT_DATE(RTC_DATE *data)
{
	uint8_t year = DecimaltoBCD(data->Year);
	uint8_t month = DecimaltoBCD(data->Month);
	uint8_t date = DecimaltoBCD(data->Date);
	uint8_t weekday = 2;
	uint32_t register_value=(year << 16)|(weekday << 13)|(month << 8)|(date << 0);
	RTC->DR = register_value;
}

void INIT_TIME(RTC_TIME *data)
{
	uint8_t Hour = DecimaltoBCD(data->Hours);
	uint8_t Minute = DecimaltoBCD(data->Minutes);
	uint8_t Second = DecimaltoBCD(00);
	//uint8_t PM = 0;//24Hour Format (bit 22)
	uint32_t register_value = (Hour << 16)|(Minute << 8)|(Second << 0);
	register_value &= ~(1U << 22);
	RTC->TR = register_value;
}

void GET_TIME(RTC_TIME *data)
{
	uint32_t values = RTC->TR;
	data->Hours = BCDtoDecimal((values >> 16) & 0XFF);
	data->Minutes = BCDtoDecimal((values >> 8) & 0XFF);
	data ->Seconds = BCDtoDecimal((values >> 0) & 0XFF);
}

void GET_DATE(RTC_DATE *data)
{
	uint32_t values = RTC->DR;
	data->Date = BCDtoDecimal((values >> 0) & 0XFF);
	data->Month = BCDtoDecimal((values >> 8) & 0X1F);
	data->WeekDay = BCDtoDecimal((values >> 13) & 0x7);
	data->Year = BCDtoDecimal((values >> 16) & 0XFF);
}
uint8_t BCDtoDecimal(uint8_t data)
{
	uint8_t value = ((data >> 4)*10 + (data & 0x0F));

    return value;
}
uint8_t DecimaltoBCD(uint8_t data)
{
	uint8_t value = (((data/10) << 4) | (data % 10));
    return value;
}
