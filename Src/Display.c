#include "Display.h"

extern uint8_t FRAME_BUFFER[1024];

void delay(uint16_t time)
{
	for(int i=0;i<(time*4000);i++);
}

/*
 * MISO,MOSI,CLK,NSS pin already configured in SPI.c
 * Here I am configuring only reset and DC
 * DC Pin
	 * 0 -> Command
	 * 1 -> Data
 * DC -> PA8
 * Reset -> PA9
 */
void PIN_INIT(void){
	//Enable clock
	AHB1ENR |= (1U << 0);

	//set output mode
	//PA8
	GPIOA_MODER |= (1U << 16);
	GPIOA_MODER &= ~(1U << 17);
	//PA9
	GPIOA_MODER |= (1U << 18);
	GPIOA_MODER &= ~(1U << 19);

}

void Send_Command(uint8_t command){
	GPIOA_ODR &= ~(1U << 8);
	SPI_Enable();
	Send_Data(&command,1);
	SPI_Disable();
}

void Display_Data(uint8_t *data,uint32_t length){
	GPIOA_ODR |= (1U << 8);
	SPI_Enable();
	DMA_SEND_DATA(data,length);

}

void Display_Init(void){

	PIN_INIT();

	reset();
	delay(10);
	set();
	delay(10);

	//Initialization Sequence
	Send_Command(SSD1306_DISPLAYOFF);
	Send_Command(SSD1306_MEMORYMODE);
	Send_Command(SSD1306_HORIZONTALADDRMODE);
	Send_Command(SSD1306_COLUMNADDR);
	Send_Command(SSD1306_PAGEADDR);
	Send_Command(SSD1306_COMSCANDEC);
	Send_Command(SSD1306_SETLOWCOLUMN);
	Send_Command(SSD1306_SETHIGHCOLUMN);
	Send_Command(SSD1306_SETSTARTLINE);
	Send_Command(SSD1306_SETCONTRAST);
	Send_Command(0xFF);
	Send_Command(SSD1306_SEGREMAP);
	Send_Command(SSD1306_NORMALDISPLAY);
	Send_Command(SSD1306_SETMULTIPLEX);
	Send_Command(0x3F);
	Send_Command(SSD1306_SETDISPLAYOFFSET);
	Send_Command(0X00);
	Send_Command(SSD1306_SETDISPLAYCLOCKDIV);
	Send_Command(0XF0);
	Send_Command(SSD1306_SETPRECHARGE);
	Send_Command(0X22);
	Send_Command(SSD1306_SETCOMPINS);
	Send_Command(0X12);
	Send_Command(SSD1306_SETVCOMDETECT);
	Send_Command(0x20);
	Send_Command(SSD1306_CHARGEPUMP);
	Send_Command(0x14);
	Send_Command(SSD1306_DISPLAYON);
}

void set_contrast(uint8_t value){
	Send_Command(SSD1306_SETCONTRAST);
	Send_Command(value);
}

void Display_Update(void) {

    Send_Command(SSD1306_COLUMNADDR);
    Send_Command(0);
    Send_Command(127);

    Send_Command(SSD1306_PAGEADDR);
    Send_Command(0);
    Send_Command(7);


    Display_Data(FRAME_BUFFER, 1024);
}

void Display_StopScroll(void) {
    Send_Command(SSD1306_DEACTIVATE_SCROLL);
}


void Display_ScrollRight(uint8_t start_page, uint8_t end_page) {
    Display_StopScroll();

    Send_Command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
    Send_Command(0x00);
    Send_Command(start_page);
    Send_Command(0x07);
    Send_Command(end_page);
    Send_Command(0x00);
    Send_Command(0xFF);

    Send_Command(SSD1306_ACTIVATE_SCROLL);
}

void Display_ScrollLeft(uint8_t start_page, uint8_t end_page) {
    Display_StopScroll();

    Send_Command(SSD1306_LEFT_HORIZONTAL_SCROLL);
    Send_Command(0x00);
    Send_Command(start_page);
    Send_Command(0x07);
    Send_Command(end_page);
    Send_Command(0x00);
    Send_Command(0xFF);

    Send_Command(SSD1306_ACTIVATE_SCROLL);
}
void reset(void){
	GPIOA_ODR &= ~(1U << 9);
}

void set(void){
	GPIOA_ODR |= (1U << 9);
}
