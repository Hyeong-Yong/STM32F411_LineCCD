/*
 * cdc.h
 *
 *  Created on: 2022. 1. 3.
 *      Author: HYJH
 */

#include "hw_def.h"

#ifndef SRC_COMMON_HW_INCLUDE_CDC_H_
#define SRC_COMMON_HW_INCLUDE_CDC_H_

#ifdef _USE_HW_CDC
bool     cdcInit(void);
bool	 cdcIsInit(void);
uint32_t cdcAvailable(void);//ring buffer
uint8_t  cdcRead(void);
uint8_t  cdcDataIn(uint8_t rx_data);
uint8_t  cdcWrite(uint8_t *p_data, uint32_t length);
uint32_t cdcGetBaud(void);

#endif

#endif /* SRC_COMMON_HW_INCLUDE_CDC_H_ */
