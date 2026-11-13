/*
 * Display.cpp
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: maksi
 */

#include "Display.h"

extern I2C_HandleTypeDef hi2c1;


Display::Display(I2C_HandleTypeDef* hi2c, xQueueHandle TransferDataQueue)
{
	this->hi2c = hi2c;
	this->TransferDataQueue = TransferDataQueue;
	this->curr_state = Display_is_rebooting;
}
void Display::Init()
{
	HAL_StatusTypeDef stat;
	this->address = 0;
	// Finding the address of the LED Display
	for (int i = 0; i < 127; ++i)
	{
		this->address = (i << 1);
		stat = HAL_I2C_IsDeviceReady(this->hi2c, this->address, 1, 10);
		if (stat == HAL_OK)
		{
			break;
		}
	}
}

void Display::see_the_speed(uint32_t freq)
{
	switch (freq)
	{
		case Really_Fast:
			ssd1306_WriteString("Really Fast", Font_7x10, Black);
			return;
		case Relatively_Fast:
			ssd1306_WriteString("R-ly Fast", Font_7x10, Black);
			return;
		case Slow:
			ssd1306_WriteString("Slow", Font_7x10, Black);
			return;
		case Really_Slow:
			ssd1306_WriteString("Really Slow", Font_7x10, Black);
			return;
	}
}

void Display::DisplayTaskMain()
{
	DisplayPayload receivedData;

	receivedData.clicks_num = 0;
	receivedData.clicks_per_ms = 8;
	receivedData.on_flag = 0;
	receivedData.is_configuring = 0;

	while(1)
	{
		if(xQueueReceive(this->TransferDataQueue, &receivedData, 0) == pdTRUE)
		{
			if (this->curr_state == Display_is_fine || this->curr_state == Display_is_configuring)
			{
                if (receivedData.is_configuring == 1)
                {
                    this->curr_state = Display_is_configuring;
                } else {
                    this->curr_state = Display_is_fine;
                }
            }
		}
		switch (this->curr_state)
		{
			case Display_is_rebooting:
				if (HAL_I2C_IsDeviceReady(&hi2c1, address, 2, 100) == HAL_OK)
				{
					ssd1306_Init(&hi2c1);
					ssd1306_Fill(Black);
					ssd1306_UpdateScreen(&hi2c1);
					this->curr_state = Display_is_fine;
				}
				else
				{
					this->curr_state = Display_is_lost;
				}
				break;
			case Display_is_fine:
			{
				if (HAL_I2C_IsDeviceReady(&hi2c1, address, 2, 100)  == HAL_OK)
				{
					if (receivedData.on_flag == 0)
					{
						ssd1306_Fill(White);
						ssd1306_SetCursor(25, 4);
						ssd1306_WriteString("Autoclicker", Font_7x10, Black);
						ssd1306_SetCursor(0, 18);
						ssd1306_WriteString("State: turned off", Font_7x10, Black);
						ssd1306_SetCursor(0, 32);
						ssd1306_WriteString("To change it", Font_7x10, Black);
						ssd1306_SetCursor(0, 46);
						ssd1306_WriteString("Use B1 User Button", Font_7x10, Black);
						ssd1306_UpdateScreen(&hi2c1);
						break;
					}
					char str_buf[16];
					ssd1306_Fill(White);
					ssd1306_SetCursor(25, 4);
					ssd1306_WriteString("Autoclicker", Font_7x10, Black);
					ssd1306_SetCursor(0, 18);
					ssd1306_WriteString("State: turned on", Font_7x10, Black);
					ssd1306_SetCursor(0, 34);
					ssd1306_WriteString("Clicks made:", Font_7x10, Black);
					snprintf(str_buf, sizeof(str_buf), "%lu",receivedData.clicks_num);
					ssd1306_WriteString(str_buf, Font_7x10, Black);
					ssd1306_SetCursor(0, 48);
					ssd1306_WriteString("Frequency(ms): ", Font_7x10, Black);
					snprintf(str_buf, sizeof(str_buf), "%lu",receivedData.clicks_per_ms);
					ssd1306_WriteString(str_buf, Font_7x10, Black);
					ssd1306_UpdateScreen(&hi2c1);
					break;
				}
				else
				{
					this->curr_state = Display_is_lost;
				}
				break;
			}
			case Display_is_lost:
				if (HAL_I2C_IsDeviceReady(&hi2c1, address, 2, 100)  == HAL_OK)
				{
					vTaskDelay(1000);
					this->curr_state = Display_is_rebooting;
				}
				break;
			case Display_is_configuring:
			{
				if (HAL_I2C_IsDeviceReady(&hi2c1, address, 2, 100)  == HAL_OK)
				{
					char str_buf[16];
					ssd1306_Fill(White);
					ssd1306_SetCursor(25, 4);
					ssd1306_WriteString("Autoclicker", Font_7x10, Black);
					ssd1306_SetCursor(0, 18);
					ssd1306_WriteString("State: Configuring", Font_7x10, Black);
					ssd1306_SetCursor(0, 34);
					snprintf(str_buf, sizeof(str_buf), "%lu", receivedData.clicks_per_ms);
					ssd1306_WriteString("Frequency(ms): ", Font_7x10, Black);
					ssd1306_WriteString(str_buf, Font_7x10, Black);
					ssd1306_SetCursor(0, 50);
					ssd1306_WriteString("Mode: ", Font_7x10, Black);
					see_the_speed(receivedData.clicks_per_ms);
					ssd1306_UpdateScreen(&hi2c1);
					break;
				}
				else
				{
					this->curr_state = Display_is_lost;
				}
				break;
			}
		}
		vTaskDelay(100);
	}
}

void Display::StartTask(uint32_t priority)
{
    xTaskCreate(
        DisplayTaskWrapper,
        "DisplayTask",
        512,
        this,
        priority,
        &taskHandle
    );
}

void Display::DisplayTaskWrapper(void* pvParameters)
{
    Display* instance = static_cast<Display*>(pvParameters);
    instance->DisplayTaskMain();
}
