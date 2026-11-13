/*
 * DataTransfer.h
 *
 *  Created on: 11 нояб. 2026 г.
 *      Author: maksi
 */

#ifndef INC_DATATRANSFER_H_
#define INC_DATATRANSFER_H_

#include <stdint.h>

typedef struct {
	uint32_t volatile clicks_num;
	uint8_t volatile on_flag;
	uint32_t volatile clicks_per_ms;
	uint8_t is_configuring;
} DisplayPayload;


#endif /* INC_DATATRANSFER_H_ */
