#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#define STM32F446xx
#include "stm32f4xx.h"
#include <stdint.h>

void Interrupt_INIT(void);
#define EXTI10_IRQn		40U
#define EXTI5_IRQn		23U
#define EXTI4_IRQn		10U
#define EXTI3_IRQn		9U
#define EXTI1_IRQn		7U
#define EXTI0_IRQn		6U
#endif /* INTERRUPT_H_ */
