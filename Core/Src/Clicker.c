/*
 * Clicker.c
 *
 *  Created on: 28 сент. 2026 г.
 *      Author: maksi
 */

#include <stdint.h>
#include "Clicker.h"

void Clicker_ctor(clicker_info * const me,
		uint32_t clicks_num,
		volatile uint8_t on_flag,
		uint32_t clicks_per_ms) {
	me->clicks_num = clicks_num;
	me->on_flag = on_flag;
	me->clicks_per_ms = clicks_per_ms;
}
