/*
 * Display.h
 *
 *  Created on: 4 нояб. 2026 г.
 *      Author: maksi
 */

#ifndef DISPLAY_H
#define DISPLAY_H

extern "C" {
    #include "ssd1306.h"
}
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "DataTransfer.h"
#include <stdio.h>

class Display
{
private:
	enum DisplayState {
		Display_is_rebooting = 0,
		Display_is_fine,
		Display_is_lost,
		Display_is_configuring
	};
	enum Click_Frequences {
		Really_Fast = 8,
		Relatively_Fast = 50,
		Slow = 100,
		Really_Slow = 200
	};
	I2C_HandleTypeDef* hi2c;
	uint8_t address;
	QueueHandle_t TransferDataQueue;

	DisplayState curr_state;

	TaskHandle_t taskHandle;

	static void DisplayTaskWrapper(void* pvParameters);
	void DisplayTaskMain();
	void see_the_speed(uint32_t freq);
public:
	Display(I2C_HandleTypeDef* hi2c, QueueHandle_t TransferDataQueue);
	void Init();
	void StartTask(uint32_t priority);
};

#endif // DISPLAY_H
