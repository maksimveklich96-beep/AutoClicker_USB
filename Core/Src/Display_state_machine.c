/*
 * Display_state_machine.c
 *
 *  Created on: 4 окт. 2026 г.
 *      Author: maksi
 */

#include "Display_state_machine.h"
#include "Clicker.h"
#include "i2c.h"
#include "ssd1306.h"
#include "cmsis_os.h"

extern DisplayState display;
extern clicker_info curr_clicker;

void Control_Display(uint8_t address, uint8_t flag)
{

	switch (display)
	{
		case Display_is_rebooting:
			if (HAL_I2C_IsDeviceReady(&hi2c1, address, 2, 100) == HAL_OK)
			{
				ssd1306_Init(&hi2c1);
				ssd1306_Fill(Black);
				ssd1306_UpdateScreen(&hi2c1);
				display = Display_is_fine;
			}
			else
			{
				display= Display_is_lost;
			}
			break;
		case Display_is_fine:
			if (HAL_I2C_IsDeviceReady(&hi2c1, address, 2, 100)  == HAL_OK)
			{
				if (flag == 0)
				{
					ssd1306_Fill(Black);
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
				snprintf(str_buf, sizeof(str_buf), "%lu", curr_clicker.clicks_num);
				ssd1306_WriteString(str_buf, Font_7x10, Black);
				ssd1306_SetCursor(0, 48);
				ssd1306_WriteString("Frequency(ms): ", Font_7x10, Black);
				snprintf(str_buf, sizeof(str_buf), "%lu",curr_clicker.clicks_per_ms);
				ssd1306_WriteString(str_buf, Font_7x10, Black);
				ssd1306_UpdateScreen(&hi2c1);
			}
			else
			{
				display = Display_is_lost;
			}
			break;
		case Display_is_lost:
			if (HAL_I2C_IsDeviceReady(&hi2c1, address, 2, 100)  == HAL_OK)
			{
				osDelay(1000);
				display = Display_is_rebooting;
			}
			break;
	}
}
