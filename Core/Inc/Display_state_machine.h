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

typedef enum {
	Display_is_rebooting = 0,
	Display_is_fine,
	Display_is_lost
} DisplayState;

void Control_Display(uint8_t address, uint8_t flag);

#endif /* INC_DISPLAY_STATE_MACHINE_H_ */
