/*
 * tim.h
 *
 *  Created on: May 24, 2022
 *      Author: HYJH
 */

#ifndef SRC_COMMON_HW_INCLUDE_PWM_H_
#define SRC_COMMON_HW_INCLUDE_PWM_H_

#include "hw_def.h"

#define PWM_MAX_CH	HW_PWM_MAX_CH


#ifdef _USE_HW_PWM


bool pwmInit(void);
bool pwmBegin(uint8_t ch, uint32_t period);
bool pwmDeinit(uint8_t ch);
void pwmStart(uint8_t ch);
bool pwmStop(uint8_t ch);
void pwmSycDelay(uint8_t ch, uint32_t delay);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

void CCDSingleShot();
bool CCDset(uint32_t SH, uint32_t ICG);

#endif


#endif /* SRC_COMMON_HW_INCLUDE_PWM_H_ */
