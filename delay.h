/*
 *******************************************************************************
 * @file           : delay.h
 * @brief          : header file for delay.h
 * masks and defines
 * project         : EE 329 S'26 A3
 * authors         : Joseph Matella and Tyler Ragasa
 * version         : 0.1
 * date            : 20260421
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "stm32l4xx_hal.h"

void SysTick_Init(void);
void delay_us(const uint32_t time_us);

#endif /* INC_DELAY_H_ */
