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
char temp_str[32];
char pressure_str[32];
uint8_t count = 0;
uint8_t sensor_select = 0;
RTC_DATE Date;
RTC_TIME Time;
int32_t t_int = 0;
int32_t t_dec = 0;
uint32_t p_pa = 0;
uint32_t p_int = 0;
uint32_t p_dec = 0;

uint8_t contrast = 0;
uint8_t navigation = 0;
uint8_t old_navigation = 255;
void SysTick_Callback(void);

int main(void)
{
	uart2_tx_init();
    SPI1_GPIO_INIT();
    SPI_Master_INIT();
    SPI_DMA_INIT();
    Display_Init();
	I2C_INIT();
	I2C_INTERRUPT_EN();
	RTC_INIT();
	ADC_INIT();
	one_sec_delay();
	Interrupt_INIT();
	I2C_ENABLE();
	if(BMP280_Init()){
	    printf("BMP280 Sensor OK\r\n");
		draw_text(0,16,16,"BMP: OK ");
		Display_Update();
		for(int i = 0; i< 2000;i++);
	}else{
	    printf("BMP280 Sensor NOT FOUND\r\n");
		draw_text(0,16,16,"BMP:ERR");
		Display_Update();
		for(int i = 0; i< 2000;i++);
		while(1);
	}

	int32_t temp;
	uint32_t press;
	while(1)
	{
		if(contrast == 0){
			memset(FRAME_BUFFER, 0, FB_SIZE);
			switch(navigation)
			{
				case 0:
					draw_text(0,16,16,"HOME");
				    break;

				case 1:
				   draw_text(0,16,16,"ALARM");
				   break;

				case 2:
				   draw_text(0,16,16,"TIMER");
				   break;
			}
			Display_Update();

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
		}else{
	    	ADC_START();
	        uint8_t adc = ADC_value();
	        ADC_STOP();
	        set_contrast(adc);

		}
	}
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
		printf("Ack failure\n");

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
        for(int i = 0; i< 300000;i++); //debounce
    }
}
