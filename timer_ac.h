/*
 *******************************************************************************
 * @file           : timer.h
 * @brief          : header file for Parts A-C TIM2 waveform support
 * project         : EE 329 S'26 A4 Parts A-C
 * authors         : Joseph Matella and Gabriel Rouse
 * version         : 1.0
 * date            : 20260430
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics. All rights reserved.
 *******************************************************************************
 * Description:
 * Defines GPIO mappings, constants, and function prototypes for Parts A-C
 * waveform generation, ISR timing measurement, and MCO support.
 *
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 *******************************************************************************
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#define WAVEFORM_OUT GPIO_PIN_5
#define ISR_TIMING_BIT GPIO_PIN_6

// 800 cycle period corresponds to 5 kHz
// Lowest period without breaking is 146
#define PERIOD 146

#define DUTY_CYCLE 400

void Init_Interrupt_Pin(void);
void setup_TIM2(int iDutyCycle);
void TIM2_IRQHandler(void);
void setup_MCO_CLK(void);




#endif /* INC_TIMER_H_ */
