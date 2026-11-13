/*
 * Clicker.h
 *
 *  Created on: 28 сент. 2026 г.
 *      Author: maksi
 */

#ifndef INC_CLICKER_H_
#define INC_CLICKER_H_

extern "C" {
    #include "ssd1306.h"
}
#include "FreeRTOS.h"
#include "task.h"
#include "usbd_hid.h"
#include "usb_device.h"
#include "queue.h"
#include "DataTransfer.h"

class Clicker
{
private:
	enum Click_Frequences {
		Really_Fast = 8,
		Relatively_Fast = 50,
		Slow = 100,
		Really_Slow = 200
		};

	uint8_t config_mode;
	uint32_t volatile clicks_num;
	uint8_t volatile on_flag;
	uint32_t volatile clicks_per_ms;
	uint8_t volatile prev_state;
	TaskHandle_t taskHandle;

	uint8_t pressed_on_array[4];
	uint8_t pressed_off_array[4];

	TaskHandle_t usbTaskHandle;
	TaskHandle_t buttonTaskHandle;

	QueueHandle_t TransferDataQueue;

	static void UsbTaskWrapper(void* pvParameters);
	void UsbTaskMain();

	static void ButtonTaskWrapper(void* pvParameters);
	void ButtonTaskMain();
public:
	Clicker(QueueHandle_t TransferDataQueue);
	void StartTask();
	void next_speed();
};

#endif /* INC_CLICKER_H_ */
