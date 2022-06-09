/*
 * hw.c
 *
 *  Created on: Dec 31, 2021
 *      Author: HYJH
 */

#include "hw.h"

void hwInit(void)
{
  bspInit();
  cliInit();
  ledInit();
  cdcInit();
  usbInit();
  uartInit();
  buttonInit();

  if (sdInit()==true)
    {fatfsInit();}

  if (buttonGetPressed(_DEF_BUTTON1) == true && sdIsDetected() == true)
    {usbBegin(USB_MSC_MODE);}
  else
    {usbBegin(USB_CDC_MODE);}


  gpioInit();
  spiInit();
  delay(500);
  adcInit();
  pwmInit();

}


