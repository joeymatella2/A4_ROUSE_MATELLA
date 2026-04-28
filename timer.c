/*
 *******************************************************************************
 * @file           : timer.c
 * @brief          : X
 * project         : EE 329 S'26 AX
 * authors         : joeym
 * version         : 0.1
 * date            : Apr 27, 2026
 * compiler        : STM32CubeIDE v.1.19.0 Build: 14980_20230301_1550 (UTC)
 * target          : NUCLEO-L4A6ZG
 * clocks          : 4 MHz MSI to AHB2
 * @attention      : (c) 2026 STMicroelectronics.  All rights reserved.
 *******************************************************************************
 * Description: X
 *
 *******************************************************************************
 * GPIO Wiring
 * |   Component    | GPIO Identifier | Connector Location | Config
 *-----------------------------------------------------------------------------
 * | LCD - DB4 - 11 | PC0             | CN9-3              | OUT
 *******************************************************************************
 * Version History
 *  Ver.|   Date   |  Description
 *  ---------------------------------------------------------------------------
 *      |          | 
 *******************************************************************************
 *
 * Header format adapted from [Code Appendix by Kevin Vo] pg 5
 */

#include "timer.h"
#include "main.h"

void Init_Interrupt_Pin(void) {

	RCC -> AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);

  	/* Configure GPIO pins 5 and 6 as outputs, push-pull, very high speed,
  	 * no pull-up or pull-down
  	 */
  	GPIOA -> MODER &= ~(GPIO_MODER_MODE5
  			              | GPIO_MODER_MODE6);

  	GPIOA -> MODER |= (GPIO_MODER_MODE5_0
  			             | GPIO_MODER_MODE6_0);

  	GPIOA -> OTYPER  &= ~(GPIO_OTYPER_OT5
  			                 | GPIO_OTYPER_OT6);

  	GPIOA -> OSPEEDR |= (GPIO_OSPEEDR_OSPEED5
  			               | GPIO_OSPEEDR_OSPEED6);

  	GPIOA -> PUPDR &= ~(GPIO_PUPDR_PUPD5
  			               | GPIO_PUPDR_PUPD6);

   // reset pins to LOW
  	GPIOA -> BRR = (WAVEFORM_OUT
  			           | ISR_TIMING_BIT);
}


void setup_TIM2(void) {
   RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;           // enable clock for TIM2
   TIM2->ARR = 0xFFFFFFFF;                         // Set ARR to max
   TIM2->CCR1 = 0x2625A00;                         // ticks for 10s
   TIM2->CNT = 0;                                  // Reset Count
   NVIC->ISER[0] |= (1 << (TIM2_IRQn & 0x1F));     // set NVIC interrupt: 0x1F
   __enable_irq();                                 // global IRQ enable
}

void start_TIM2(void) {
   TIM2->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE);  // enable event gen, rcv CCR1
   TIM2->CNT = 0;                                  // Reset Count
   TIM2->SR &= ~(TIM_SR_CC1IF | TIM_SR_UIF);       // clr IRQ flag in status reg
   TIM2->CR1 |= TIM_CR1_CEN;                       // start TIM2 CR1
}

void stop_TIM2(void) {
   TIM2->CR1 &= ~TIM_CR1_CEN;                       // STOP TIM2 CR1
   TIM2->DIER &= ~(TIM_DIER_CC1IE | TIM_DIER_UIE);  // disable event gen, rcv CCR1
}

int readCount_TIM2(void) {
	return TIM2->CNT;
}

void setup_MCO_CLK(void) {
   // enable MCO, MCOSEL = 0b0001 to select SYSCLK = MSI (4 MHz source)
   RCC->CFGR = ((RCC->CFGR & ~(RCC_CFGR_MCOSEL)) | (RCC_CFGR_MCOSEL_0));
   // configure MCO output on PA8
   RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOAEN);
   GPIOA->MODER   &= ~(GPIO_MODER_MODE8);    	// clear MODER bits to set ...
   GPIOA->MODER   |=  (GPIO_MODER_MODE8_1);	// MODE = alternate function
   GPIOA->OTYPER  &= ~(GPIO_OTYPER_OT8);     	// push-pull output
   GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPD8);    	// pullup & pulldown OFF
   GPIOA->OSPEEDR |=  (GPIO_OSPEEDR_OSPEED8);   // high speed
   GPIOA->AFR[1]  &= ~(GPIO_AFRH_AFSEL8);    	// select MCO (AF0) on PA8 (AFRH)
}

// Note: if square wave not output, press reset button on nucleo board
void TIM2_IRQHandler(void) {
   if (TIM2->SR & TIM_SR_CC1IF) {      // triggered by CCR1 event ...
      TIM2->SR &= ~(TIM_SR_CC1IF);     // manage the flag
      currentState = RESET_STATE;
      stop_TIM2();
   }
   if (TIM2->SR & TIM_SR_UIF) {        // triggered by ARR event ...
      TIM2->SR &= ~(TIM_SR_UIF);       // manage the flag
   }

}



