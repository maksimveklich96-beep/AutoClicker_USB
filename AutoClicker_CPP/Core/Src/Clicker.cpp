/*
 * Clicker.c
 *
 *  Created on: 28 сент. 2026 г.
 *      Author: maksi
 */

#include "Clicker.h"

extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t pressed_on_array[4] = {0x01, 0, 0, 0};
uint8_t pressed_off_array[4] = {0x00, 0, 0, 0};

Clicker::Clicker(QueueHandle_t TransferDataQueue)
{
	this->prev_state = 0;
	this->clicks_num = 0;
	this->clicks_per_ms = Really_Fast;
	this->on_flag = 0;
	this->config_mode = 0;
	this->TransferDataQueue = TransferDataQueue;
}

void Clicker::UsbTaskMain()
{
	while(1)
	{
		if (this->on_flag != 0) {

			while (USBD_HID_SendReport(&hUsbDeviceFS, pressed_on_array, 4) == USBD_BUSY) {
				vTaskDelay(1);
			}
			vTaskDelay(this->clicks_per_ms);
			while (USBD_HID_SendReport(&hUsbDeviceFS, pressed_off_array, 4) == USBD_BUSY) {

				vTaskDelay(1);
			}
			++this->clicks_num;
			DisplayPayload MyMessage;

			MyMessage.clicks_num = this->clicks_num;
			MyMessage.clicks_per_ms = this->clicks_per_ms;
			MyMessage.on_flag = this->on_flag;
			MyMessage.is_configuring = this->config_mode;

			xQueueSend(this->TransferDataQueue, &MyMessage, 0);

			vTaskDelay(this->clicks_per_ms);
		} else {

			vTaskDelay(10);
		}
	}
}


void Clicker::ButtonTaskMain()
{
	uint8_t button_pressed = 0;
	uint8_t long_click = 0;
	uint32_t init_time;
	while(1)
	{
		if ((HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET))
		{
			if (button_pressed == 0)			{
				long_click = 0;
				button_pressed = 1;
				init_time = xTaskGetTickCount();
			}
			else if (button_pressed == 1)
			{
				if (((xTaskGetTickCount() - init_time) > 1000) && long_click == 0)
				{
					this->config_mode = !this->config_mode;

					if (this->config_mode == 1)
					{
						this->on_flag = 0;
					    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
					}
					else
					{
					    this->on_flag = this->prev_state;
					    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, (GPIO_PinState)this->prev_state);
					 }
				     DisplayPayload MyMessage;
					 MyMessage.clicks_num = this->clicks_num;
					 MyMessage.clicks_per_ms = this->clicks_per_ms;
					 MyMessage.on_flag = this->on_flag;
					 MyMessage.is_configuring = this->config_mode;

					 xQueueSend(this->TransferDataQueue, &MyMessage, 0);

					 long_click = 1;
				}
			}
		}
		else
		{
			if (button_pressed == 1)
			{
				if (this->config_mode == 1 && long_click == 0)
				{
				    next_speed();

				    DisplayPayload MyMessage;
				    MyMessage.clicks_num = this->clicks_num;
				    MyMessage.clicks_per_ms = this->clicks_per_ms;
				    MyMessage.on_flag = this->on_flag;
				    MyMessage.is_configuring = this->config_mode;
				    xQueueSend(this->TransferDataQueue, &MyMessage, 0);
				}
				else if (this->config_mode == 0 && long_click == 0)
				{
				    this->on_flag = !this->on_flag;
				    this->prev_state = this->on_flag;
				    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

				    DisplayPayload MyMessage;
				    MyMessage.clicks_num = this->clicks_num;
				    MyMessage.clicks_per_ms = this->clicks_per_ms;
				    MyMessage.on_flag = this->on_flag;
				    MyMessage.is_configuring = this->config_mode;
				    xQueueSend(this->TransferDataQueue, &MyMessage, 0);
				}
			button_pressed = 0;
			}
		}
		vTaskDelay(20);
	}
}

void Clicker::StartTask()
{
	xTaskCreate(
		ButtonTaskWrapper,
		"ButtonTask",
		256,
		this,
		3,
		&buttonTaskHandle
	);

	xTaskCreate(
		UsbTaskWrapper,
		"UsbTask",
		256,
		this,
		2,
		&usbTaskHandle
	);
}

void Clicker::ButtonTaskWrapper(void* pvParameters)
{
	Clicker* instance = static_cast<Clicker*>(pvParameters);

	instance->ButtonTaskMain();
}

void Clicker::UsbTaskWrapper(void* pvParameters)
{
	Clicker* instance = static_cast<Clicker*>(pvParameters);

	instance->UsbTaskMain();
}


void Clicker::next_speed()
{
	switch (this->clicks_per_ms)
	{
		case Really_Fast:
			this->clicks_per_ms = Relatively_Fast;
			return;
		case Relatively_Fast:
			this->clicks_per_ms = Slow;
			return;
		case Slow:
			this->clicks_per_ms = Really_Slow;
			return;
		case Really_Slow:
			this->clicks_per_ms = Really_Fast;
			return;
		default:
			this->clicks_per_ms = Really_Slow;
			return;
	}
}
