/*
 * usb.c
 *
 *  Created on: 2022. 1. 2.
 *      Author: HYJH
 */

#include "usb.h"
#include "usb_device.h"

#ifdef _USE_HW_USB


bool usbInit(void)
{
  bool ret = true;

  MX_USB_DEVICE_Init();

  return ret;
}


#endif
