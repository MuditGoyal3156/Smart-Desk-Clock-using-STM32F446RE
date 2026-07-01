#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>
#include "SPI.h"
#include "OLED.h"

#define RCC_BASE_ADDR		0x40023800U
#define AHB1ENR		(*(volatile uint32_t* )(RCC_BASE_ADDR + 0x30U))

#define GPIOA_BASE_ADDR		0x40020000U
#define GPIOA_MODER			(*(volatile uint32_t* )(GPIOA_BASE_ADDR + 0x00U))
#define GPIOA_ODR			(*(volatile uint32_t* )(GPIOA_BASE_ADDR + 0x14U))

void delay(uint16_t time);
void PIN_INIT(void);
void Send_Command(uint8_t command);
void Display_Data(uint8_t *data,uint32_t length);
void Display_Init(void);
void Display_Update(void);
void Display_StopScroll(void);
void set_contrast(uint8_t value);
void Display_ScrollRight(uint8_t start_page, uint8_t end_page);
void Display_ScrollLeft(uint8_t start_page, uint8_t end_page);
void reset(void);
void set(void);

//Commands
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_HORIZONTALADDRMODE 0x00
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETCOMPINS 0x12
#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40



#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3





#endif /* DISPLAY_H_ */
