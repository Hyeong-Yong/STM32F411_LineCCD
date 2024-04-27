/*
 * hw.h
 *
 *  Created on: Dec 31, 2021
 *      Author: HYJH
 */

#ifndef SRC_HW_HW_H_
#define SRC_HW_HW_H_

#include <pwm.h>
#include "hw_def.h"


#include "led.h"
#include "uart.h"
#include "usb.h"
#include "reset.h"
#include "rtc.h"
#include "cdc.h"
#include "flash.h"
#include "cli.h"
#include "button.h"
#include "gpio.h"
#include "sd.h"
#include "fatfs.h"
#include "spi.h"
#include "adc.h"


void hwInit(void);


#endif /* SRC_HW_HW_H_ */