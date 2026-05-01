/*
 *******************************************************************************
 * @file           : main.c
 * @brief          : main program for EE 329 Assignment 4 reaction timer
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
 * Main application for the reaction timer assignment. Initializes the system
 * clock, LCD, delay functions, button input, LED output, RNG, MCO output, and
 * TIM2. Implements the reaction timer state machine with RESET, DELAY, TIME,
 * UPDATE, and CHEAT states.
 *
 *******************************************************************************
 * GPIO Wiring
 * |   Component    | GPIO Identifier | Connector Location | Config
 *-----------------------------------------------------------------------------
 * | Pushbutton     | PC13            | User Button        | IN
 * | Reaction LED   | PB7             | GPIO output        | OUT
 * | MCO Clock Out  | PA8             | CN10-23            | AF0
 *******************************************************************************
 * Version History
 *  Ver.|   Date   |  Description
 *  ---------------------------------------------------------------------------
 *  1.0 | 20260430 | Finalized reaction timer main program for A4
 *******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lcd.h"
#include "delay.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
volatile button_state_t current_state = STATE_RESET;
static uint32_t clock_ticks = 0;
static uint32_t final_time = 0;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
// main program entry point.
// initializes clocks, peripherals, lcd, rng, and timer modules.
// runs the reaction timer state machine forever.
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  SysTick_Init();
  setup_MCO_CLK();
  setup_TIM2();
  LCD_Init();
  PB_Init();
  LED_Init();
  RNG_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  // read current pushbutton state once per loop iteration
	  uint8_t button_input = BUTTON_PRESSED();
	  switch (current_state) {

	  // reset state:
	  // turn LED off, show prompt, and display the previous reaction time.
	  // wait here until the user presses the button to begin a new trial.
	  	case STATE_RESET:
	  		LED_OFF();
	  		LCD_Clear();
	  		LCD_Set_Cursor(0, 0);
	  		LCD_Write_String("PUSH SW TO TRIG");
	  		LCD_Set_Cursor(1, 0);
	  		LCD_Write_String("TIME: ");
	  		LCD_Write_Digit((final_time / 1000) % 10);
	  		LCD_Write_String(".");
	  		LCD_Write_Digit((final_time / 100) % 10);
	  		LCD_Write_Digit((final_time / 10) % 10);
	  		LCD_Write_Digit((final_time) % 10);

	  		LCD_Write_String(" s");
	  		// Poll button press
	  		while (!button_input) {
	  			button_input = BUTTON_PRESSED();
	  		}
	  		// Button pressed, go to delay state
	  		current_state = STATE_DELAY;
	  		break;
	  		// delay state:
	  		// display standby message and wait a random amount of time before
	  		// turning on the LED. if the button is pressed during this delay,
	  		// treat it as cheating.
	  	case STATE_DELAY:
	  		LCD_Clear();
	  		LCD_Set_Cursor(0, 0);
	  		LCD_Write_String("Watch For LED");
	  		// Generate random number for delay
	  		uint32_t rand_delay = RNG_Read();
	  		delay_us(rand_delay);
	  		// If button held down before LED press, cheat detected
	  		button_input = BUTTON_PRESSED();
	  		if (button_input) {
	  			current_state = STATE_CHEAT;
	  		} else {
	  			start_TIM2();
	  			LED_ON();
	  			current_state = STATE_TIME;
	  		}
	  		break;

	  		// timing state:
	  		// once the LED is on and TIM2 is running, wait for the user to press
	  		// the button. when pressed, stop the timer and save the elapsed count.
	  	case STATE_TIME:
	  		// Poll button press
	  		if (button_input) {
	  		     stop_TIM2();
	  		     clock_ticks = readCount_TIM2();
	  		     // Wait for button to be released
	  		     while(button_input) {
	  		   	  button_input = BUTTON_PRESSED();
	  		     }
	  		     // Debounce
	  		     delay_us(10000);
	  		     current_state = STATE_UPDATE;
	  		    }
	  		break;
	  		// update state:
	  		// convert raw timer counts into milliseconds and save the result
	  		// for display during the next reset state.
	  	case STATE_UPDATE:
	  		LED_OFF();
	  		// Time in miliseconds
	  		final_time = clock_ticks / 4000;

	  		current_state = STATE_RESET;
	  		break;
	  		// cheat state:
	  		// if the button is pressed before the LED turns on, display a
	  		// cheating message, wait briefly, then return to reset.
	  	case STATE_CHEAT:
	  		  		LCD_Clear();
	  		  		LCD_Set_Cursor(0, 0);
	  		  		LCD_Write_String("YOU ARE A CHEAT!");
	  		  		LCD_Set_Cursor(1, 0);
	  		  		delay_us(2000000);
		  		     // Wait for button to be released
		  		     while(button_input) {
		  		   	  button_input = BUTTON_PRESSED();
		  		     }
		  		     // Debounce
		  		     delay_us(10000);
	  		  		current_state = STATE_RESET;
	  		  		break;
	  	default:
	  		current_state = STATE_RESET;
	  		break;
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
}

/* -----------------------------------------------------------------------------
 * function : PB_Init()
 * INs      : none
 * OUTs     : none
 * action   : configures PC13 as a digital input for the user pushbutton
 * -------------------------------------------------------------------------- */
void PB_Init(void) {
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOCEN);
	BUTTON_PORT->MODER &= ~(GPIO_MODER_MODE13);
	BUTTON_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD13);
	BUTTON_PORT->PUPDR |= (GPIO_PUPDR_PUPD13_1);
}
/* -----------------------------------------------------------------------------
 * function : LED_Init()
 * INs      : none
 * OUTs     : none
 * action   : configures PB7 as a push-pull digital output for the reaction LED
 * -------------------------------------------------------------------------- */
void LED_Init(void) {
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOBEN);
	LED_PORT->MODER &= ~(GPIO_MODER_MODE7);
	LED_PORT->MODER |= (GPIO_MODER_MODE7_0);
	LED_PORT->OTYPER &= ~(GPIO_OTYPER_OT7);
	LED_PORT->PUPDR &= ~(GPIO_PUPDR_PUPD7);
	LED_PORT->OSPEEDR |= (GPIO_OSPEEDR_OSPEED7);

	// Reset Pin
	LED_PORT->BRR |= (LED_PIN);
}
/* -----------------------------------------------------------------------------
 * function : LED_ON()
 * INs      : none
 * OUTs     : none
 * action   : sets the LED output pin high to turn the reaction LED on
 * -------------------------------------------------------------------------- */
void LED_ON(void) {
	LED_PORT->BSRR |= (LED_PIN);
}
/* -----------------------------------------------------------------------------
 * function : LED_OFF()
 * INs      : none
 * OUTs     : none
 * action   : clears the LED output pin to turn the reaction LED off
 * -------------------------------------------------------------------------- */
void LED_OFF(void) {
	LED_PORT->BRR |= (LED_PIN);
}
/* -----------------------------------------------------------------------------
 * function : BUTTON_PRESSED()
 * INs      : none
 * OUTs     : returns button state as bool
 * action   : reads the pushbutton input and returns its current logic level
 * -------------------------------------------------------------------------- */
bool BUTTON_PRESSED(void) {
	// Button is active low
	return ((BUTTON_PORT->IDR >> 13) & 1);
}
/* -----------------------------------------------------------------------------
 * function : RNG_Init()
 * INs      : none
 * OUTs     : none
 * action   : enables the hardware random number generator and its 48 MHz clock
 * -------------------------------------------------------------------------- */
void RNG_Init(void) {
	// Enable clock
	RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
	// Enable external peripheral clock
	RCC->CRRCR |= RCC_CRRCR_HSI48ON;
	// Select clock source
	RCC->CCIPR &= ~RCC_CCIPR_CLK48SEL;
	// Enable random number generator
	RNG->CR |= RNG_CR_RNGEN;
}
/* -----------------------------------------------------------------------------
 * function : RNG_Read()
 * INs      : none
 * OUTs     : returns uint32_t random delay value
 * action   : reads one random number from the RNG peripheral and scales it to
 *            a delay range of 1,000,000 us to 5,000,000 us
 * -------------------------------------------------------------------------- */
uint32_t RNG_Read(void) {
		RCC->CRRCR |= RCC_CRRCR_HSI48ON; /* make sure the 48 MHz clock source for RNG is enabled
		*/
	    while ((RNG->SR & RNG_SR_DRDY) == 0) { // wait until random data is ready
	        ; // nop
	    }
	    // read one 32-bit random number from the data register
	    // scale result to 1 to 5 seconds in microseconds
	    uint32_t uiRand = RNG->DR;    // 32-bit RN.
	    uiRand = (uiRand % 4000001U) + 1000000U; // scale number (1 - 5 seconds)
	    return uiRand;
}




/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
