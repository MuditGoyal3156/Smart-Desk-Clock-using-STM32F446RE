#include <stdio.h>
#include <stdint.h>
#include "SYSTIC.h"
#include "uart.h"
#include "DHT22.h"
#include "fonts.h"
#include "SPI.h"
#include "Display.h"
#include "OLED.h"
#include "BMP280.h"
#include "I2C.h"
#include "RTC.h"
#include <string.h>
#include "ADC.h"
#include "Interrupt.h"

extern uint8_t FRAME_BUFFER[FB_SIZE];

DHT_DATA sensor;
ALARM_DATE Data;
ALARM_TIME time;
RTC_DATE Date;
RTC_TIME Time;

volatile uint8_t set_date=0;
volatile uint8_t alarm_date = 1;
volatile uint8_t alarm_hour = 0;
volatile uint8_t alarm_minute = 0;
volatile uint8_t time_select = 0;
volatile uint8_t timer = 0;

volatile uint8_t date = 1;
volatile uint8_t month = 1;
volatile uint8_t year = 0;
volatile uint8_t hour = 0;
volatile uint8_t minute = 0;


char temp_str[32];
char pressure_str[32];
volatile uint8_t count = 0;
volatile uint8_t sensor_select = 0;


volatile int32_t t_int = 0;
volatile int32_t t_dec = 0;
volatile uint32_t p_pa = 0;
volatile uint32_t p_int = 0;
volatile uint32_t p_dec = 0;


volatile uint8_t contrast = 0;
volatile uint8_t navigation = 0;


void BUZZ_INIT(void);
void BUZZ_OFF(void);
void BUZZ_ON(void);
void SysTick_Callback(void);

int main(void)
{
	uart2_tx_init();
    SPI1_GPIO_INIT();
    SPI_Master_INIT();
    SPI_DMA_INIT();
    Display_Init(0xFF);
	I2C_INIT();
	BUZZ_INIT();
	I2C_INTERRUPT_EN();
	Date.Date = 1;
	Date.Month = 1;
	Date.Year = 00;

	Time.Hours = 0;
	Time.Minutes = 0;
	Time.Seconds = 0;
	RTC_INIT(&Time,&Date);
	ADC_INIT();
	one_sec_delay();
	Interrupt_INIT();
	I2C_ENABLE();
	if(BMP280_Init()){
	    printf("BMP280 Sensor OK\r\n");
	}else{

		while(1){
			printf("BMP280 Sensor NOT FOUND\r\n");
			 for(int i = 0; i< 20000;i++);
		}
	}

	int32_t temp;
	uint32_t press;
	memset(FRAME_BUFFER, 0, FB_SIZE);
	while(1)
	{
		if(contrast == 0){

			switch(navigation)
			{
				case 0:
					if(set_date == 0){
						if(count % 2 == 0)
						{
							if(sensor_select == 0)
							{
								DHT22_DATA(&sensor);
								GET_DATE(&Date);
								GET_TIME(&Time);
							}
							else
							{
								GET_TIME(&Time);

								temp  = BMP280_ReadTemperature();
								press = BMP280_ReadPressure();

								t_int = temp / 100;
								t_dec = temp % 100;

								p_pa  = press / 256;
								p_int = p_pa / 100;
								p_dec = p_pa % 100;

								sprintf(temp_str,"Temperature: %ld.%02ld C",t_int,t_dec);
								sprintf(pressure_str,"Pressure: %ld.%02ld hPa",p_int,p_dec);
							}
						}
					}
				    char line1[24];
				    char line2[24];
				    char line3[24];
				    char line4[24];

				    sprintf(line1,"Date: %02d/%02d/%02d", Date.Date, Date.Month, Date.Year);
				    sprintf(line2,"Time: %02d:%02d", Time.Hours, Time.Minutes);
				    sprintf(line3,"Temp: %ld.%02ldC",t_int,t_dec);
				    sprintf(line4,"Hum:  %d.%d %%", sensor.humidity/10,sensor.humidity%10);

				    draw_text(48,0,8,"HOME");
				    draw_text(0,16,8,line1);
				    draw_text(0,28,8,line2);
				    draw_text(0,40,8,line3);
				    draw_text(0,52,8,line4);
				    break;

				case 1:
				    char HOUR_DISPLAY[24];
				    char MINUTE_DISPLAY[24];

				    sprintf(HOUR_DISPLAY,"Hour: %02d", alarm_hour);
				    sprintf(MINUTE_DISPLAY,"Minute: %02d",alarm_minute);

				    draw_text(44,0,8,"ALARM");
				    draw_text(0,28,8,HOUR_DISPLAY);
				    draw_text(0,40,8,MINUTE_DISPLAY);

				   break;

				case 2:
				    char TIMER[24];

				    sprintf(TIMER,"%02d", timer);
				   draw_text(44,0,8,"TIMER");
				   draw_text(44,28,16,TIMER);
				   break;
			}
			Display_Update();
			for(int i = 0; i< 20000;i++);
			memset(FRAME_BUFFER, 0, FB_SIZE);


		}else{
	    	ADC_START();
	        uint8_t adc = ADC_value();
	        ADC_STOP();
	        char line[24];
	        memset(FRAME_BUFFER, 0, FB_SIZE);
	        draw_text(0,0,8,"CONTRAST");
	        sprintf(line,"Value: %d", adc);
	        draw_text(0,16,8,line);
	        set_contrast(adc);
	        Display_Update();
	        for(int i = 0; i< 2000;i++);
	        memset(FRAME_BUFFER, 0, FB_SIZE);
		}
	}
}
//PB6
void BUZZ_INIT(void){
	//Clock already enabled

	//Set PB6 as output
	GPIOB->MODER &= ~(3U << 12);
	GPIOB->MODER |= (1U << 12);


}

void BUZZ_OFF(void){
	//Write 0 to PB6
	GPIOB->ODR &= ~(1U << 6);

}
void BUZZ_ON(void){
	//Write 1 to PB6
	GPIOB->ODR |= (1U << 6);

}
void SysTick_Callback(void)
{
	count++;

    if(count == 6)
    {
        sensor_select = 1;
    }
    else if(count == 12)
    {
        sensor_select = 0;
        count = 0;
    }
}
void SysTick_Handler(void)
{
	SysTick_Callback();
}




void I2C1_EV_IRQHandler(void)
{
	I2C_EV_HANDLE(&I2C_Handle);
}

void I2C1_ER_IRQHandler(void)
{
	I2C_ER_Handler(&I2C_Handle);
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t AppEv)
{
	if(AppEv == I2C_ERROR_BERR){
        printf("I2C Bus Error\r\n");

	}else if(AppEv == I2C_ERROR_ARLO){

        printf("I2C Arbitration Lost\r\n");

	}else if(AppEv == I2C_ERROR_AF)
	{
		printf("Ack failure\r\n");

		I2C_CloseSendData(&I2C_Handle);

		Generate_Stop();

		while(1);
	}
	else if(AppEv == I2C_ERROR_OVR){

        printf("I2C Overrun Error\r\n");

        I2C_CloseReceiveData(pI2CHandle);

        Generate_Stop();

        while(1);
	}
}

void EXTI1_IRQHandler(void){
    if (EXTI->PR & (1U << 1))
    {
        // Clear the pending bit
        EXTI->PR |= (1U << 1);

        // Toggle system state
        contrast = !contrast;

        // Simple debounce delay
        for(int i = 0; i< 200000;i++); //debounce
    }
}

void EXTI0_IRQHandler(void){
    if (EXTI->PR & (1U << 0))
    {
        // Clear the pending bit
        EXTI->PR |= (1U << 0);

        // Toggle system state
        navigation = ( navigation +1 ) % 3;

        // Simple debounce delay
        for(int i = 0; i< 250000;i++); //debounce
    }
}

void RTC_Alarm_IRQHandler(void)
{
    if(RTC->ISR & (1U << 8))
    {
        RTC->ISR &= ~(1U << 8);

        EXTI->PR = (1U << 17);

        printf("ALARM SET\r\n");
        BUZZ_ON();

    }
}

void EXTI3_IRQHandler(void){

    if (EXTI->PR & (1U << 3))
    {
        // Clear the pending bit
        EXTI->PR |= (1U << 3);

        BUZZ_OFF();

       alarm_date = 0;
       alarm_hour = 0;
       alarm_minute = 0;
       Data.Date = alarm_date;
       time.Hours = alarm_hour;
       time.Minutes = alarm_minute;
       Alarm_INIT(&time, &Data);

        // Simple debounce delay
        for(int i = 0; i< 250000;i++); //debounce
    }

}
void EXTI4_IRQHandler(void){

    if (EXTI->PR & (1U << 4))
    {
        // Clear the pending bit
        EXTI->PR |= (1U << 4);
        if(navigation==0){
        	time_select = (time_select + 1) % 5;
        }else if(navigation==1){
        	time_select = (time_select + 1) % 2;
        }


        // Simple debounce delay
        for(int i = 0; i< 250000;i++); //debounce
    }
}
void EXTI9_5_IRQHandler(void){

    if (EXTI->PR & (1U << 5))
    {
        // Clear the pending bit
        EXTI->PR |= (1U << 5);

        if(navigation == 0){
        	set_date = 1;
        	switch(time_select){
        	case 0:
        		date++;
        		if(date > 31)
        			date = 1;
        		break;
        	case 1:
        		month++;
        		if(month >12)
        			month = 1;
        		break;
        	case 2:
        		year = (year +1)%100;
        		break;
        	case 3:
        		hour = (hour + 1)%24;
        		break;
        	case 4:
        		minute = (minute + 1)%60;
        		break;
        	}

        	Date.Date = date;
        	Date.Month = month;
        	Date.Year = year;
        	Time.Hours = hour;
			Time.Minutes = minute;
			Time.Seconds = 0;
        }else if(navigation == 1){
        	switch(time_select){
        	case 0:
        		alarm_hour = (alarm_hour + 1) % 24;
        		break;
        	case 1:
        		alarm_minute = (alarm_minute + 1) % 60;
        		break;
        	}

        	time.Hours = alarm_hour;
			time.Minutes = alarm_minute;
			time.Seconds = 5;
        }else if(navigation == 2){
        	timer = (timer+1)%61;
        }


        // Simple debounce delay
        for(int i = 0; i< 250000;i++); //debounce
    }

}

void EXTI15_10_IRQHandler(void){

    if (EXTI->PR & (1U << 10))
    {
        // Clear the pending bit
        EXTI->PR |= (1U << 10);
        if(navigation == 0){
        	RTC_INIT(&Time,&Date);
        	set_date = 0;
        	for(int i = 0; i< 20000;i++);

        }else if(navigation == 1){
        	Alarm_INIT(&time, &Data);
        }else if(navigation == 2){

        }

        // Simple debounce delay
        for(int i = 0; i< 250000;i++); //debounce
    }

}
