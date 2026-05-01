/*
 *******************************************************************************
 * @file           : delay.h
 * @brief          : header file for delay.c
 * project         : EE 329 S'26 A4
 * authors         : Joseph Matella and Gabriel Rouse
 * version         : 1.0
 * date            : 20260430
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics. All rights reserved.
 *******************************************************************************
 * Description:
 * Declares SysTick-based microsecond delay support functions used throughout
 * the reaction timer application.
 *
 *******************************************************************************
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 *******************************************************************************
 */
#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include "stm32l4xx_hal.h"

void SysTick_Init(void);
void delay_us(const uint32_t time_us);

#endif /* INC_DELAY_H_ */
