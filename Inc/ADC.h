#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>


void ADC_GPIO_INIT(void);
void ADC_INIT(void);
void ADC_START(void);
void ADC_STOP(void);
uint8_t ADC_value(void);

#endif
