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
	one_sec_delay();
	I2C_ENABLE();
	if(BMP280_Init())
	    printf("BM280 Sensor OK\r\n");
	else
	    printf("BM280 Sensor NOT FOUND\r\n");


	int32_t temp;
	uint32_t press;
    while(1)
    {
    	draw_text(0,1,16,"Hello");
    	Display_Update();
    	uS_delay(100);
    	if(count%2 == 0){

			if(sensor_select == 0){
				DHT22_DATA(&sensor);
		    	GET_DATE(&Date);
		    	GET_TIME(&Time);


			}else if(sensor_select == 1){
		    	GET_TIME(&Time);
				temp = BMP280_ReadTemperature();
				press = BMP280_ReadPressure();

				//Calculations to process values of temperature and pressure
				t_int = temp / 100;
				t_dec = temp % 100;
				p_pa = press / 256;
				p_int = p_pa / 100;
				p_dec = p_pa % 100;

				sprintf(temp_str, "Temperature: %ld.%02ld C", t_int, t_dec);
				sprintf(pressure_str, "Pressure: %ld.%02ld hPa", p_int, p_dec);

			}
			printf("%s \r\n",temp_str);
			printf("%s \r\n",pressure_str);
			printf("Temp = %d.%dC\r\n", (sensor.temperature/10),(sensor.temperature%10));
			printf("Humidity = %d.%d%%\r\n",(sensor.humidity/10),(sensor.humidity%10));
		    printf("Date:%d/%d/%d\n\r",Date.Date,Date.Month,Date.Year);
		    printf("Time: %d:%d:%d\n\r",Time.Hours,Time.Minutes,Time.Seconds);
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
