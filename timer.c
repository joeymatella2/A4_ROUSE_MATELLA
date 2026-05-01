/*
 *******************************************************************************
 * @file           : timer.c
 * @brief          : TIM2 and MCO support for reaction timer operation
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
 * Configures TIM2 as a free-running reaction timer and timeout source for the
 * final reaction timer game. Also provides MCO clock output setup for external
 * oscilloscope measurement of the 4 MHz system clock.
 *
 *******************************************************************************
 * GPIO Wiring
 * |   Component    | GPIO Identifier | Connector Location | Config
 *-----------------------------------------------------------------------------
 * | MCO Clock Out  | PA8             | CN10-23            | AF0
 *******************************************************************************
 * Version History
 *  Ver.|   Date   |  Description
 *  ---------------------------------------------------------------------------
 *  1.0 | 20260430 | Finalized reaction timer TIM2 control module
 *******************************************************************************
 */
#include "timer.h"
#include "main.h"

/* -----------------------------------------------------------------------------
 * function : setup_TIM2()
 * INs      : none
 * OUTs     : none
 * action   : configures TIM2 as a free-running timer with CCR1 set for a
 *            10-second timeout and enables the TIM2 interrupt in the NVIC
 * -------------------------------------------------------------------------- */
void setup_TIM2(void) {
   RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;           // enable clock for TIM2
   TIM2->ARR = 0xFFFFFFFF;                         // Set ARR to max
   TIM2->CCR1 = 0x2625A00;                         // ticks for 10s
   TIM2->CNT = 0;                                  // Reset Count
   NVIC->ISER[0] |= (1 << (TIM2_IRQn & 0x1F));     // set NVIC interrupt: 0x1F
   __enable_irq();                                 // global IRQ enable
}

/* -----------------------------------------------------------------------------
 * function : start_TIM2()
 * INs      : none
 * OUTs     : none
 * action   : resets TIM2 count, clears pending flags, enables interrupts, and
 *            starts the timer for a new reaction timing interval
 * -------------------------------------------------------------------------- */
void start_TIM2(void) {
   TIM2->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE);  // enable event gen, rcv CCR1
   TIM2->CNT = 0;                                  // Reset Count
   TIM2->SR &= ~(TIM_SR_CC1IF | TIM_SR_UIF);       // clr IRQ flag in status reg
   TIM2->CR1 |= TIM_CR1_CEN;                       // start TIM2 CR1
}

/* -----------------------------------------------------------------------------
 * function : stop_TIM2()
 * INs      : none
 * OUTs     : none
 * action   : stops TIM2 and disables its compare and update interrupts
 * -------------------------------------------------------------------------- */
void stop_TIM2(void) {
   TIM2->CR1 &= ~TIM_CR1_CEN;                       // STOP TIM2 CR1
   TIM2->DIER &= ~(TIM_DIER_CC1IE | TIM_DIER_UIE);  // disable event gen, rcv CCR1
}

/* -----------------------------------------------------------------------------
 * function : readCount_TIM2()
 * INs      : none
 * OUTs     : returns current TIM2 count value
 * action   : reads the current counter value from TIM2 for reaction timing
 * -------------------------------------------------------------------------- */
uint32_t readCount_TIM2(void) {
	return TIM2->CNT;
}

/* -----------------------------------------------------------------------------
 * function : setup_MCO_CLK()
 * INs      : none
 * OUTs     : none
 * action   : configures PA8 as MCO output and routes the 4 MHz system clock
 *            to that pin for oscilloscope measurement
 * -------------------------------------------------------------------------- */
void setup_MCO_CLK(void) {
   // enable MCO, MCOSEL = 0b0001 to select SYSCLK = MSI (4 MHz source)
   RCC->CFGR = ((RCC->CFGR & ~(RCC_CFGR_MCOSEL)) | (RCC_CFGR_MCOSEL_0));
   // configure MCO output on PA8
   RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOAEN);
   GPIOA->MODER   &= ~(GPIO_MODER_MODE8);    	// clear MODER bits to set ...
   GPIOA->MODER   |=  (GPIO_MODER_MODE8_1);	   // MODE = alternate function
   GPIOA->OTYPER  &= ~(GPIO_OTYPER_OT8);     	// push-pull output
   GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPD8);    	// pullup & pulldown OFF
   GPIOA->OSPEEDR |=  (GPIO_OSPEEDR_OSPEED8);   // high speed
   GPIOA->AFR[1]  &= ~(GPIO_AFRH_AFSEL8);    	// select MCO (AF0) on PA8 (AFRH)
}

/* -----------------------------------------------------------------------------
 * function : TIM2_IRQHandler()
 * INs      : none
 * OUTs     : none
 * action   : handles TIM2 compare and update interrupts; on CCR1 timeout the
 *            reaction timer is stopped and the program returns to reset state
 * -------------------------------------------------------------------------- */
void TIM2_IRQHandler(void) {
   if (TIM2->SR & TIM_SR_CC1IF) {      // triggered by CCR1 event ...
      TIM2->SR &= ~(TIM_SR_CC1IF);     // manage the flag
      current_state = STATE_RESET;
      stop_TIM2();
   }
   if (TIM2->SR & TIM_SR_UIF) {        // triggered by ARR event ...
      TIM2->SR &= ~(TIM_SR_UIF);       // manage the flag
   }
}
