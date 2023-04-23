/*
 * adc.h
 *
 *  Created on: May 24, 2022
 *      Author: HYJH
 */

#ifndef SRC_COMMON_HW_INCLUDE_ADC_H_
#define SRC_COMMON_HW_INCLUDE_ADC_H_

#include "hw_def.h"

#ifdef _USE_HW_ADC

#define ADC_MAX_CH HW_ADC_MAX_CH

void adcInit(void);
void ADCstart_DMA(uint8_t ch);
void ADCstop_DMA(uint8_t ch);

#endif

#endif /* SRC_COMMON_HW_INCLUDE_ADC_H_ */
