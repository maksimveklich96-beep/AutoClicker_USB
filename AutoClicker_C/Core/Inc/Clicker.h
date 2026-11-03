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
	uint8_t volatile prev_state;
} clicker_info;

void Clicker_ctor(clicker_info * const me,
		uint32_t clicks_num,
		uint8_t volatile on_flag,
		uint32_t clicks_per_ms,
		uint8_t volatile prev_state);

typedef enum {
	Really_Fast = 8,
	Relatively_Fast = 50,
	Slow = 100,
	Really_Slow = 200
} Click_Frequences;

#endif /* INC_CLICKER_H_ */
