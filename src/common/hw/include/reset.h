/*
 * reset.h
 *
 *  Created on: 2022. 1. 3.
 *      Author: HYJH
 */

#ifndef SRC_COMMON_HW_INCLUDE_RESET_H_
#define SRC_COMMON_HW_INCLUDE_RESET_H_

#include "hw_def.h"

#ifdef _USB_HW_RESET

bool resetInit(void);
uint32_t resetGetCount(void);

#endif

#endif /* SRC_COMMON_HW_INCLUDE_RESET_H_ */
