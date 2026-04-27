/*
 * timer.h
 *
 *  Created on: Apr 24, 2026
 *      Author: joeym
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#define WAVEFORM_OUT GPIO_PIN_5
#define IN_ISR GPIO_PIN_6

// 800 cycle period corresponds to 5 kHz
#define PERIOD 800

#define DUTY_CYCLE 400

void Init_Interrupt_Pin(void);
void setup_TIM2(int iDutyCycle);
void TIM2_IRQHandler(void);
void setup_MCO_CLK(void);




#endif /* INC_TIMER_H_ */
