/*
 * rtc.c
 *
 *  Created on: 2022. 1. 3.
 *      Author: HYJH
 */

#include "rtc.h"
//Backup Register 활성하시켜줘서 Reset button 누를 시 데이터를 BackupRegister에 저장


#ifdef _USE_HW_RTC

static RTC_HandleTypeDef hrtc;

bool rtcInit(void)
{
  bool ret=true;
  __HAL_RCC_GPIOC_CLK_ENABLE();
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  return ret;
}

void rtcBackupRegWrite(uint32_t index, uint32_t data)
{
  HAL_RTCEx_BKUPWrite(&hrtc, index, data);
}


uint32_t rtcBackupRegRead(uint32_t index)
{
  uint32_t ret;
  ret = HAL_RTCEx_BKUPRead(&hrtc, index);
  return ret;
}



void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}


#endif
