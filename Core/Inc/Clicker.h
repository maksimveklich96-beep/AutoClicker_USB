/*
 * Clicker.h
 *
 *  Created on: 28 сент. 2026 г.
 *      Author: maksi
 */

#ifndef INC_CLICKER_H_
#define INC_CLICKER_H_

typedef struct {
	uint32_t volatile clicks_num;
	uint8_t volatile on_flag;
	uint32_t volatile clicks_per_ms;
} clicker_info;

void Clicker_ctor(clicker_info * const me,
		uint32_t clicks_num,
		uint8_t volatile on_flag,
		uint32_t clicks_per_ms);

#endif /* INC_CLICKER_H_ */
