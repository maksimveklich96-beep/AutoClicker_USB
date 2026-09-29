/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "Clicker.h"
#include "usb_device.h"
#include "usbd_hid.h"
#include "i2c.h"
#include "ssd1306.h"

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
/* USER CODE BEGIN Variables */

extern clicker_info curr_clicker;
extern USBD_HandleTypeDef hUsbDeviceFS;

osThreadId_t ButtonHandle;

const osThreadAttr_t ButtonTask_attributes = {
  .name = "buttonTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t DisplayHandle;

const osThreadAttr_t DisplayTask_attributes = {
  .name = "DisplayTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

uint8_t pressed_on_array[4] = {0x01, 0, 0, 0};
uint8_t pressed_off_array[4] = {0x00, 0, 0, 0};

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void StartButtonTask(void *argument);

void StartDisplayTask(void *argument);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  ButtonHandle = osThreadNew(StartButtonTask, NULL, &ButtonTask_attributes);

  DisplayHandle = osThreadNew(StartDisplayTask, NULL, &DisplayTask_attributes);
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  uint8_t button_pressed = 0;
  for(;;)
  {
	  if ((HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET))
	  {
		  if (button_pressed == 0)
		  {
			  button_pressed = 1;
			  curr_clicker.on_flag = !curr_clicker.on_flag;
		  	  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		  }
	  }
	  else
	  {
		button_pressed = 0;
	  }
	  osDelay(20);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void StartButtonTask(void *argument)
{

  /* Infinite loop */
  for(;;)
  {
    if (curr_clicker.on_flag != 0) {

	    while (USBD_HID_SendReport(&hUsbDeviceFS, pressed_on_array, 4) == USBD_BUSY) {
	    	osDelay(1);
	    }
	    osDelay(curr_clicker.clicks_per_ms);

	    while (USBD_HID_SendReport(&hUsbDeviceFS, pressed_off_array, 4) == USBD_BUSY) {
	    	osDelay(1);
	    }
	    ++curr_clicker.clicks_num;
	    osDelay(curr_clicker.clicks_per_ms);
    } else {

	    osDelay(10);
	}
  }
}

void StartDisplayTask(void *argument)
{
	char str_buf[16];
	ssd1306_Init(&hi2c1);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen(&hi2c1);
	for(;;)
	{
		if (curr_clicker.on_flag != 0)
		{	ssd1306_Fill(White);
			ssd1306_SetCursor(25, 4);
			ssd1306_WriteString("Autoclicker", Font_7x10, Black);
			ssd1306_SetCursor(0, 18);
			ssd1306_WriteString("State: turned on", Font_7x10, Black);
			ssd1306_SetCursor(0, 34);
			ssd1306_WriteString("Clicks made:", Font_7x10, Black);
			snprintf(str_buf, sizeof(str_buf), "%lu", curr_clicker.clicks_num);
			ssd1306_WriteString(str_buf, Font_7x10, Black);
			ssd1306_SetCursor(0, 48);
			ssd1306_WriteString("Frequency(ms): ", Font_7x10, Black);
			snprintf(str_buf, sizeof(str_buf), "%lu",curr_clicker.clicks_per_ms);
			ssd1306_WriteString(str_buf, Font_7x10, Black);
			ssd1306_UpdateScreen(&hi2c1);
			osDelay(100);
		}
		else
		{
			ssd1306_Fill(Black);
			ssd1306_UpdateScreen(&hi2c1);
		}
	}
}

/* USER CODE END Application */

