/*
 *******************************************************************************
 * @file           : timer.c
 * @brief          : TIM2 waveform generation and ISR timing support for A4
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
 * Configures TIM2 for Parts A through C waveform generation experiments.
 * Supports square-wave generation, ISR timing measurement, and MCO clock
 * output for oscilloscope reference measurements.
 *
 *******************************************************************************
 * GPIO Wiring
 * |   Component       | GPIO Identifier | Connector Location | Config
 *-----------------------------------------------------------------------------
 * | Waveform Output   | PA5             | CN10-11            | OUT
 * | ISR Timing Bit    | PA6             | CN10-13            | OUT
 * | MCO Clock Out     | PA8             | CN10-23            | AF0
 *******************************************************************************
 * Version History
 *  Ver.|   Date   |  Description
 *  ---------------------------------------------------------------------------
 *  1.0 | 20260430 | Finalized Parts A-C timer experiment module
 *******************************************************************************
 */
#include "timer.h"

/* -----------------------------------------------------------------------------
 * function : Init_Interrupt_Pin()
 * INs      : none
 * OUTs     : none
 * action   : configures PA5 and PA6 as digital outputs for waveform output and
 *            ISR timing measurement
 * -------------------------------------------------------------------------- */
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


/* -----------------------------------------------------------------------------
 * function : setup_TIM2()
 * INs      : iDutyCycle = initial compare value for CCR1
 * OUTs     : none
 * action   : configures TIM2 for waveform generation, enables TIM2 interrupts,
 *            loads ARR and CCR1 values, and starts the timer
 * -------------------------------------------------------------------------- */
void setup_TIM2(int iDutyCycle) {
   RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;           // enable clock for TIM2
   TIM2->DIER |= (TIM_DIER_CC1IE | TIM_DIER_UIE);  // enable event gen, rcv CCR1
   TIM2->ARR = 0xFFFFFFFF;                             // ARR = T = counts @4MHz
   TIM2->CCR1 = iDutyCycle;                        // ticks for duty cycle
   TIM2->SR &= ~(TIM_SR_CC1IF | TIM_SR_UIF);       // clr IRQ flag in status reg
   NVIC->ISER[0] |= (1 << (TIM2_IRQn & 0x1F));     // set NVIC interrupt: 0x1F
   __enable_irq();                                 // global IRQ enable
   TIM2->CR1 |= TIM_CR1_CEN;                       // start TIM2 CR1
}

/* -----------------------------------------------------------------------------
 * function : setup_MCO_CLK()
 * INs      : none
 * OUTs     : none
 * action   : configures PA8 as MCO output and routes the 4 MHz system clock
 *            to that pin for oscilloscope reference measurement
 * -------------------------------------------------------------------------- */
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


/* -----------------------------------------------------------------------------
 * function : TIM2_IRQHandler()
 * INs      : none
 * OUTs     : none
 * action   : services TIM2 interrupts by toggling the waveform output at CCR1,
 *            updating CCR1 for the next edge, and pulsing the ISR timing bit
 * -------------------------------------------------------------------------- */
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


