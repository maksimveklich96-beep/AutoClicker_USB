/*
 * app.cpp
 *
 *  Created on: 3 нояб. 2026 г.
 *      Author: maksi
 */

#include "app.h"
#include "DataTransfer.h"
#include "Clicker.h"
#include "Display.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;
Clicker* myClicker;
Display* myDisplay;

QueueHandle_t TransferDataQueue;

void app_main(void) {
	TransferDataQueue = xQueueCreate(5, sizeof(DisplayPayload));

	if (TransferDataQueue != NULL)
	{
		myClicker = new Clicker(TransferDataQueue);
		myDisplay = new Display(&hi2c1, TransferDataQueue);

		myDisplay->Init();

		myClicker->StartTask();
		myDisplay->StartTask(1);
	}
}
