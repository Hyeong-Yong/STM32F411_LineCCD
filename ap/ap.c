/*
 * ap.c
 *
 *  Created on: Dec 31, 2021
 *      Author: HYJH
 */


#include "ap.h"
#include "lcd/st7735.h"


void apInit(void)
{
  cliOpen(_DEF_UART2, 57600);
  st7735Init();
  gpioPinWrite(1, _DEF_HIGH);

//  pwmStart(_DEF_PWM1);
//  pwmSycDelay(_DEF_PWM1, 1); //1 count => 500 ns
  pwmStart(_DEF_PWM2);
//  pwmStart(_DEF_PWM3);
  pwmStart(_DEF_PWM4);
//  ADCstart_DMA(_DEF_ADC1);
}

void apMain(void)
{
  uint32_t pre_time;
  pre_time=millis();

  while(1)
    {
      if (millis()-pre_time >=500)
        {
          pre_time=millis();
          ledToggle(_DEF_LED1);
        }

      cliMain();
    }
}

