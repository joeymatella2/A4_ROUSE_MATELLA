/*******************************************************************************
* EE 329 A3 LCD INTERFACE
*******************************************************************************
* @file    : lcd.c
* @brief   : 16x2 parallel LCD driver in 4-bit write-only mode
* project  : EE 329 Assignment 3
* authors  : Gabriel Rouse and Joseph Matella
* version  : 1.0
* date     : 260420
******************************************************************************/

#include "lcd.h"
#include "delay.h"

/* -----------------------------------------------------------------------------
 * function : LCD_Write_Nibble()
 * INs      : nibble = lower 4 bits to place on PD3..PD0
 * OUTs     : none
 * action   : writes one 4-bit value to the LCD data pins
 * -------------------------------------------------------------------------- */
static void LCD_Write_Nibble( uint8_t nibble ) {
   LCD_PORT->ODR &= ~( LCD_DATA_BITS );
   LCD_PORT->ODR |= (uint32_t)( nibble & 0x0F );
}

/* -----------------------------------------------------------------------------
 * function : LCD_Status_LED_Config()
 * INs      : none
 * OUTs     : none
 * action   : configures PD6 as output for countdown-complete LED
 * -------------------------------------------------------------------------- */
void LCD_Status_LED_Config( void ) {
   LCD_PORT->MODER &= ~( GPIO_MODER_MODE8 );
   LCD_PORT->MODER |= ( GPIO_MODER_MODE8_0 );

   LCD_PORT->OTYPER &= ~( GPIO_OTYPER_OT8 );
   LCD_PORT->PUPDR &= ~( GPIO_PUPDR_PUPD8 );
   LCD_PORT->OSPEEDR |= ( 3U << GPIO_OSPEEDR_OSPEED8_Pos );

   LCD_PORT->BRR = LCD_STATUS_LED;
}

/* -----------------------------------------------------------------------------
 * function : LCD_Status_LED_On()
 * INs      : none
 * OUTs     : none
 * action   : turns on status LED on PD6
 * -------------------------------------------------------------------------- */
void LCD_Status_LED_On( void ) {
   LCD_PORT->BSRR = LCD_STATUS_LED;
}

/* -----------------------------------------------------------------------------
 * function : LCD_Status_LED_Off()
 * INs      : none
 * OUTs     : none
 * action   : turns off status LED on PD6
 * -------------------------------------------------------------------------- */
void LCD_Status_LED_Off( void ) {
   LCD_PORT->BRR = LCD_STATUS_LED;
}

/* -----------------------------------------------------------------------------
 * function : LCD_Pulse_ENA()
 * INs      : none
 * OUTs     : none
 * action   : pulses LCD enable line, latching current data on falling edge
 * -------------------------------------------------------------------------- */
void LCD_Pulse_ENA( void ) {
   LCD_PORT->ODR |= LCD_EN;
   delay_us( 5U );
   LCD_PORT->ODR &= ~( LCD_EN );
   delay_us( 5U );
}

/* -----------------------------------------------------------------------------
 * function : LCD_4b_Command()
 * INs      : command = 8-bit command, high nibble only is sent
 * OUTs     : none
 * action   : used during lcd wake-up sequence before 4-bit mode is established
 * -------------------------------------------------------------------------- */
void LCD_4b_Command( uint8_t command ) {
   LCD_PORT->ODR &= ~( LCD_RS );
   LCD_Write_Nibble( (uint8_t)( command >> 4 ) );
   delay_us( 5U );
   LCD_Pulse_ENA();
}

/* -----------------------------------------------------------------------------
 * function : LCD_Command()
 * INs      : command = full 8-bit LCD command
 * OUTs     : none
 * action   : sends command in 4-bit mode, high nibble then low nibble
 * -------------------------------------------------------------------------- */
void LCD_Command( uint8_t command ) {
   LCD_PORT->ODR &= ~( LCD_RS );

   LCD_Write_Nibble( (uint8_t)( command >> 4 ) );
   delay_us( 5U );
   LCD_Pulse_ENA();

   LCD_Write_Nibble( command );
   delay_us( 5U );
   LCD_Pulse_ENA();

   if (( command == LCD_CLEAR_DISPLAY ) ||
       ( command == LCD_RETURN_HOME )) {
      delay_us( 3000U );
   }
   else {
      delay_us( 50U );
   }
}

/* -----------------------------------------------------------------------------
 * function : LCD_Write_Char()
 * INs      : letter = ASCII character to display
 * OUTs     : none
 * action   : sends one character to LCD data register
 * -------------------------------------------------------------------------- */
void LCD_Write_Char( uint8_t letter ) {
   LCD_PORT->ODR |= LCD_RS;

   LCD_Write_Nibble( (uint8_t)( letter >> 4 ) );
   delay_us( 5U );
   LCD_Pulse_ENA();

   LCD_Write_Nibble( letter );
   delay_us( 5U );
   LCD_Pulse_ENA();

   LCD_PORT->ODR &= ~( LCD_RS );
   delay_us( 50U );
}

/* -----------------------------------------------------------------------------
 * function : LCD_Write_String()
 * INs      : str = null-terminated string
 * OUTs     : none
 * action   : writes a character string to the LCD
 * -------------------------------------------------------------------------- */
void LCD_Write_String( const char *str ) {
   while ( *str != '\0' ) {
      LCD_Write_Char( (uint8_t)( *str ) );
      str++;
   }
}

/* -----------------------------------------------------------------------------
 * function : LCD_Set_Cursor()
 * INs      : row = 0 or 1, col = 0..15
 * OUTs     : none
 * action   : sets LCD cursor position
 * -------------------------------------------------------------------------- */
void LCD_Set_Cursor( uint8_t row, uint8_t col ) {
   uint8_t address;

   if ( row == 0U ) {
      address = (uint8_t)( LCD_LINE_1_ADDR + col );
   }
   else {
      address = (uint8_t)( LCD_LINE_2_ADDR + col );
   }

   LCD_Command( (uint8_t)( LCD_SET_DDRAM | address ) );
}

/* -----------------------------------------------------------------------------
 * function : LCD_Clear()
 * INs      : none
 * OUTs     : none
 * action   : clears display and returns cursor to home position
 * -------------------------------------------------------------------------- */
void LCD_Clear( void ) {
   LCD_Command( LCD_CLEAR_DISPLAY );
   delay_us( 3000U );
}

/* -----------------------------------------------------------------------------
 * function : LCD_Write_Digit()
 * INs      : digit = value 0..9
 * OUTs     : none
 * action   : writes a single decimal digit to the LCD
 * -------------------------------------------------------------------------- */
void LCD_Write_Digit( uint8_t digit ) {
   LCD_Write_Char( (uint8_t)( '0' + ( digit % 10U ) ) );
}

/* -----------------------------------------------------------------------------
 * function : LCD_Init()
 * INs      : none
 * OUTs     : none
 * action   : configures GPIOD pins and initializes LCD in 4-bit mode
 * -------------------------------------------------------------------------- */
void LCD_Init( void ) {
   RCC->AHB2ENR |= LCD_GPIO_ENR;

   LCD_PORT->MODER &= ~( GPIO_MODER_MODE0 | GPIO_MODER_MODE1 |
                         GPIO_MODER_MODE2 | GPIO_MODER_MODE3 |
                         GPIO_MODER_MODE4 | GPIO_MODER_MODE5 |
                         GPIO_MODER_MODE8 );

   LCD_PORT->MODER |= ( GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 |
                        GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 |
                        GPIO_MODER_MODE4_0 | GPIO_MODER_MODE5_0 |
                        GPIO_MODER_MODE8_0 );

   LCD_PORT->OTYPER &= ~( GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 |
                          GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3 |
                          GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 |
                          GPIO_OTYPER_OT8 );

   LCD_PORT->PUPDR &= ~( GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 |
                         GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3 |
                         GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 |
                         GPIO_PUPDR_PUPD8 );

   LCD_PORT->OSPEEDR |= ( ( 3U << GPIO_OSPEEDR_OSPEED0_Pos ) |
                          ( 3U << GPIO_OSPEEDR_OSPEED1_Pos ) |
                          ( 3U << GPIO_OSPEEDR_OSPEED2_Pos ) |
                          ( 3U << GPIO_OSPEEDR_OSPEED3_Pos ) |
                          ( 3U << GPIO_OSPEEDR_OSPEED4_Pos ) |
                          ( 3U << GPIO_OSPEEDR_OSPEED5_Pos ) |
                          ( 3U << GPIO_OSPEEDR_OSPEED8_Pos ) );

   LCD_PORT->BRR = ( LCD_DATA_BITS | LCD_CTRL_BITS | LCD_STATUS_LED );

   LCD_Status_LED_Config();

   delay_us( 40000U );

   LCD_4b_Command( 0x30U );
   delay_us( 5000U );

   LCD_4b_Command( 0x30U );
   delay_us( 200U );

   LCD_4b_Command( 0x30U );
   delay_us( 200U );

   LCD_4b_Command( 0x20U );
   delay_us( 40U );

   LCD_Command( LCD_FUNCTION_SET );
   LCD_Command( LCD_DISPLAY_OFF );
   LCD_Command( LCD_CLEAR_DISPLAY );
   LCD_Command( LCD_ENTRY_MODE );
   LCD_Command( LCD_DISPLAY_ON );
}
