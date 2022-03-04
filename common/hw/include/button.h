/*
 * button.h
 *
 *  Created on: 2022. 3. 3.
 *      Author: HYJH
 */

#ifndef SRC_COMMON_HW_INCLUDE_BUTTON_H_
#define SRC_COMMON_HW_INCLUDE_BUTTON_H_

#include "hw_def.h"

#ifdef _USE_HW_BUTTON

#define BUTTON_MAX_CH	HW_BUTTON_MAX_CH


bool buttonInit(void);
bool buttonGetPressed(uint8_t ch);



#endif


#endif /* SRC_COMMON_HW_INCLUDE_BUTTON_H_ */
