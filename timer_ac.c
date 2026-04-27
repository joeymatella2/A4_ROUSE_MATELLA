/*
 * timer.c
 *
 *  Created on: Apr 24, 2026
 *      Author: joeym
 */
#include "timer.h"

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


void setup_TIM2( int iDutyCycle ) {
   RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;           // enable clock for TIM2
   TIM2->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE);  // enable event gen, rcv CCR1
   TIM2->ARR = 0xFFFFFFFF;                             // ARR = T = counts @4MHz
   TIM2->CCR1 = iDutyCycle;                        // ticks for duty cycle
   TIM2->SR &= ~(TIM_SR_CC1IF | TIM_SR_UIF);       // clr IRQ flag in status reg
   NVIC->ISER[0] |= (1 << (TIM2_IRQn & 0x1F));     // set NVIC interrupt: 0x1F
   __enable_irq();                                 // global IRQ enable
   TIM2->CR1 |= TIM_CR1_CEN;                       // start TIM2 CR1
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
	// Turn on ISR timing GPIO pin
	GPIOA -> ODR |= ISR_TIMING_BIT;
   if (TIM2->SR & TIM_SR_CC1IF) {      // triggered by CCR1 event ...
      TIM2->SR &= ~(TIM_SR_CC1IF);     // manage the flag
      // Increment capture and compare register
      TIM2->CCR1 += PERIOD / 2;
      // Toggle square wave output pin
      GPIOA -> ODR ^= WAVEFORM_OUT;
   }
   if (TIM2->SR & TIM_SR_UIF) {        // triggered by ARR event ...
      TIM2->SR &= ~(TIM_SR_UIF);       // manage the flag
   }
   // Turn off ISR timing GPIO pin
   GPIOA -> ODR &= ~ ISR_TIMING_BIT;
}


