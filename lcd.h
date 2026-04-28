/*
 * lcd.h
 *
 *  Created on: Apr 15, 2026
 *      Author: gabri
 */

#ifndef __LCD_H
#define __LCD_H

#include "stm32l4xx.h"
#include <stdint.h>

/* gpio port mapping ---------------------------------------------------------*/
#define LCD_PORT           GPIOC
#define LCD_GPIO_ENR       RCC_AHB2ENR_GPIOCEN

/* lcd control pins ----------------------------------------------------------*/
#define LCD_RS             GPIO_PIN_4
#define LCD_EN             GPIO_PIN_5

/* lcd data pins, low nibble of GPIOD ---------------------------------------*/
#define LCD_D4             GPIO_PIN_0
#define LCD_D5             GPIO_PIN_1
#define LCD_D6             GPIO_PIN_2
#define LCD_D7             GPIO_PIN_3

#define LCD_DATA_BITS      ( LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7 )
#define LCD_CTRL_BITS      ( LCD_RS | LCD_EN )

/* status led ----------------------------------------------------------------*/
#define LCD_STATUS_LED     GPIO_PIN_8

/* lcd commands --------------------------------------------------------------*/
#define LCD_CLEAR_DISPLAY  ( 0x01 )
#define LCD_RETURN_HOME    ( 0x02 )

#define LCD_ENTRY_MODE     ( 0x06 )
#define LCD_DISPLAY_ON     ( 0x0C )
#define LCD_DISPLAY_OFF    ( 0x08 )
#define LCD_CURSOR_ON      ( 0x0E )
#define LCD_CURSOR_BLINK   ( 0x0F )

#define LCD_FUNCTION_SET   ( 0x28 )  /* 4-bit, 2-line, 5x8 font */
#define LCD_SET_DDRAM      ( 0x80 )

#define LCD_LINE_1_ADDR    ( 0x00 )
#define LCD_LINE_2_ADDR    ( 0x40 )

#define LCD_LINE_1         ( 0x80 )
#define LCD_LINE_2         ( 0xC0 )

/* exported function prototypes ----------------------------------------------*/
void LCD_Init( void );
void LCD_Pulse_ENA( void );
void LCD_4b_Command( uint8_t command );
void LCD_Command( uint8_t command );
void LCD_Write_Char( uint8_t letter );
void LCD_Write_String( const char *str );
void LCD_Set_Cursor( uint8_t row, uint8_t col );
void LCD_Clear( void );
void LCD_Write_Digit( uint8_t digit );
void LCD_Status_LED_Config( void );
void LCD_Status_LED_On( void );
void LCD_Status_LED_Off( void );

#endif /* __LCD_H */
