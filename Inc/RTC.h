/*
 * RTC.h
 *
 *  Created on: 23-Jun-2026
 *      Author: mudit
 */

#ifndef RTC_H_
#define RTC_H_
#define STM32F446xx
#include "stm32f4xx.h"
#include <stdint.h>

typedef struct{
    uint8_t Date;
    uint8_t Month;
    uint8_t Year;
    uint8_t WeekDay;
}RTC_DATE;

typedef struct
{
    uint8_t Hours;
    uint8_t Minutes;
    uint8_t Seconds;
}RTC_TIME;


typedef struct{
    uint8_t Date;
}ALARM_DATE;

typedef struct
{
    uint8_t Hours;
    uint8_t Minutes;
    uint8_t Seconds;
}ALARM_TIME;
void RTC_INIT(RTC_TIME *time,RTC_DATE *date);
void INIT_DATE(RTC_DATE *data);
void INIT_TIME(RTC_TIME *data);
void GET_TIME(RTC_TIME *data);
void GET_DATE(RTC_DATE *data);
uint8_t DecimaltoBCD(uint8_t data);
uint8_t BCDtoDecimal(uint8_t data);
void Alarm_INIT(ALARM_TIME *time,ALARM_DATE *date);
void ALARM_DATE_AND_TIME(ALARM_DATE *data,ALARM_TIME *time);
#endif /* RTC_H_ */
