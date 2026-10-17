/*
 * Display_state_mchine.h
 *
 *  Created on: 4 окт. 2026 г.
 *      Author: maksi
 */

#ifndef INC_DISPLAY_STATE_MACHINE_H_
#define INC_DISPLAY_STATE_MACHINE_H_

#include <stdint.h>
#include <stdio.h>
#include <Clicker.h>

typedef enum {
	Display_is_rebooting = 0,
	Display_is_fine,
	Display_is_lost,
	Display_is_configuring
} DisplayState;

void Control_Display(uint8_t address, uint8_t flag);

void see_the_speed(uint8_t ms);

void display_all_speed(char str_buf[16]);

void next_speed(clicker_info* clicker);

#endif /* INC_DISPLAY_STATE_MACHINE_H_ */
