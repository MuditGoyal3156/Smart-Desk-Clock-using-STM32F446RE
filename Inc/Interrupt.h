#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#define STM32F446xx
#include "stm32f4xx.h"
#include <stdint.h>

void Interrupt_INIT(void);
#define EXTI1_IRQn		7U
#define EXTI0_IRQn		6U
#endif /* INTERRUPT_H_ */
