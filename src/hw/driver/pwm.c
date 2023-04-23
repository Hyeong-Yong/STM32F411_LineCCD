/*
 * tim.c
 *
 *  Created on: May 24, 2022
 *      Author: HYJH
 */

#include "pwm.h"
#include "adc.h"
#include "cli.h"

#ifdef _USE_HW_PWM

#ifdef _USE_HW_CLI
static void cliCCD(cli_args_t *args);
#endif

typedef struct
{
  TIM_HandleTypeDef* h_tim;
  uint32_t channel;
  uint32_t period;
  uint32_t pulse;
  bool is_timeInit;
  bool is_pwmInit;
} pwm_t;

  uint32_t current_SH;
  uint32_t current_ICG;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;

#define ABP_CLK 96000000
#define fM	 2000000
#define CCDpixel 3694
// 1count 500ns ICG : 10, SH : 4
// 2Mhz : 500ns*1480 = 7400us
// 7400 ms n=

static pwm_t pwm_tbl[PWM_MAX_CH] =
    {
	{&htim2, TIM_CHANNEL_1, 60000, 5*fM/1000000}, //  CHANNEL1 ICG (PA15) : pulse = 5 µs * fm
	{&htim3, TIM_CHANNEL_1, (ABP_CLK/fM), (ABP_CLK/fM)/2}, 	 //  CHANNEL1 fM (PB4)
	{&htim4, TIM_CHANNEL_3, 20000, 2*fM/1000000},   //  CHANNEL3 SH (PB8)  : pulse = 2 µs * fm
	{&htim5, TIM_CHANNEL_3, (4*ABP_CLK/fM), (ABP_CLK/fM)/2},    //  CHANNEL4 ADC (no output)
    };


bool pwmInit(void)
{
  bool ret =true;

  for (int i=0 ; i<PWM_MAX_CH; i++)
    {
      pwm_tbl[i].is_timeInit=false;
      pwm_tbl[i].is_pwmInit=false;

      pwmBegin(i, pwm_tbl[i].period);
    }

  current_SH = 20000;
  current_ICG = 60000;

#ifdef _USE_HW_CLI
  cliAdd("CCD", cliCCD);
#endif

  return ret;
}

bool pwmBegin(uint8_t ch, uint32_t period)
{
  bool ret = false;

  if (ch >= PWM_MAX_CH) return false;

  pwm_t *p_pwm = &pwm_tbl[ch];
  p_pwm->period = period-1;

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  switch(ch)
  {
    case _DEF_PWM1:
      p_pwm->h_tim=&htim2;

      htim2.Instance = TIM2;
      htim2.Init.Period = p_pwm->period;
      htim2.Init.Prescaler = (ABP_CLK/fM)-1;
      htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
      htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
      htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (HAL_TIM_Base_Init(p_pwm->h_tim) == HAL_OK)
        {
          p_pwm->is_timeInit = true;
          ret= true;
        }

        sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
        if (HAL_TIM_ConfigClockSource(p_pwm->h_tim, &sClockSourceConfig) != HAL_OK)
        {
          Error_Handler();
        }
        if (HAL_TIM_PWM_Init(p_pwm->h_tim) != HAL_OK)
        {
          Error_Handler();
        }

        sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(p_pwm->h_tim, &sMasterConfig) != HAL_OK)
        {
          Error_Handler();
        }

        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = p_pwm->pulse;
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        if (HAL_TIM_PWM_ConfigChannel(p_pwm->h_tim, &sConfigOC, p_pwm->channel) == HAL_OK)
        {
            p_pwm->is_pwmInit=true;
        }

        sConfigOC.OCMode = TIM_OCMODE_ACTIVE;
        sConfigOC.Pulse = 2;
        if (HAL_TIM_OC_ConfigChannel(p_pwm->h_tim, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
        {
          Error_Handler();
        }

        HAL_TIM_MspPostInit(p_pwm->h_tim);
      break;

    case _DEF_PWM2:
      p_pwm->h_tim=&htim3;

      htim3.Instance = TIM3;
      htim3.Init.Prescaler = 0;
      htim3.Init.Period = p_pwm->period;
      htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
      htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
      htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (HAL_TIM_Base_Init(p_pwm->h_tim) == HAL_OK)
        {
          p_pwm->is_timeInit = true;
        }
        sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
        if (HAL_TIM_ConfigClockSource(p_pwm->h_tim, &sClockSourceConfig) != HAL_OK)
        {
          Error_Handler();
        }
        if (HAL_TIM_PWM_Init(p_pwm->h_tim) != HAL_OK)
        {
          Error_Handler();
        }
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(p_pwm->h_tim, &sMasterConfig) != HAL_OK)
        {
          Error_Handler();
        }
        sConfigOC.OCMode = TIM_OCMODE_PWM1;
        sConfigOC.Pulse = p_pwm->pulse;
        sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
        sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
        if (HAL_TIM_PWM_ConfigChannel(p_pwm->h_tim, &sConfigOC, p_pwm->channel) == HAL_OK)
        {
            p_pwm->is_pwmInit=true;
        }
        HAL_TIM_MspPostInit(p_pwm->h_tim);
      break;

    case _DEF_PWM3:
      p_pwm->h_tim = &htim4;

      htim4.Instance = TIM4;
      htim4.Init.Prescaler = (ABP_CLK/fM)-1;
      htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
      htim4.Init.Period = p_pwm->period;
      htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
      htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
      if (HAL_TIM_Base_Init(p_pwm->h_tim) == HAL_OK)
      {
        p_pwm->is_timeInit = true;
      }

      if (HAL_TIM_PWM_Init(p_pwm->h_tim) == HAL_OK)
      {
	        p_pwm->is_pwmInit = true;
      }
      sSlaveConfig.SlaveMode = TIM_SLAVEMODE_TRIGGER;
      sSlaveConfig.InputTrigger = TIM_TS_ITR1;
      if (HAL_TIM_SlaveConfigSynchro(p_pwm->h_tim, &sSlaveConfig) != HAL_OK)
      {
        Error_Handler();
      }
      sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
      sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
      if (HAL_TIMEx_MasterConfigSynchronization(p_pwm->h_tim, &sMasterConfig) != HAL_OK)
      {
        Error_Handler();
      }
      sConfigOC.OCMode = TIM_OCMODE_PWM1;
      sConfigOC.Pulse = p_pwm->pulse;
      sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
      sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
      if (HAL_TIM_PWM_ConfigChannel(p_pwm->h_tim, &sConfigOC, p_pwm->channel) == HAL_OK)
      {
	  p_pwm->is_pwmInit=true;
      }

      HAL_TIM_MspPostInit(p_pwm->h_tim);
break;

    case _DEF_PWM4:
      p_pwm->h_tim=&htim5;

       htim5.Instance = TIM5;
       htim5.Init.Prescaler = 1-1;
       htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
       htim5.Init.Period = p_pwm->period;  //period
       htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
       htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
       if (HAL_TIM_Base_Init(p_pwm->h_tim) == HAL_OK)
       {
	   p_pwm->is_timeInit=true;
       }
       sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
       if (HAL_TIM_ConfigClockSource(p_pwm->h_tim, &sClockSourceConfig) != HAL_OK)
       {
         Error_Handler();
       }
       if (HAL_TIM_PWM_Init(p_pwm->h_tim) != HAL_OK)
       {
         Error_Handler();
       }
       sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
       sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
       if (HAL_TIMEx_MasterConfigSynchronization(p_pwm->h_tim, &sMasterConfig) != HAL_OK)
       {
         Error_Handler();
       }
       sConfigOC.OCMode = TIM_OCMODE_PWM1;
       sConfigOC.Pulse = p_pwm->pulse; //pulse
       sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
       sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
       if (HAL_TIM_PWM_ConfigChannel(p_pwm->h_tim, &sConfigOC, p_pwm->channel) == HAL_OK)
       {
         p_pwm->is_pwmInit=true;
       }
      break;
  }
  ret = true;
  return ret;
}



void timerAttachInterrupt(uint8_t channel, void (*func)(void))
{

}
void timerDetachInterrupt(uint8_t channel)
{

}

void pwmStart(uint8_t ch)
{
  pwm_t* p_pwm = &pwm_tbl[ch];
  HAL_TIM_PWM_Start(p_pwm->h_tim, p_pwm->channel);
}


bool pwmDeinit(uint8_t ch)
{
  bool ret = false;
  if (ch >= PWM_MAX_CH) return ret;

  pwm_t* p_pwm = &pwm_tbl[ch];

  if (HAL_TIM_PWM_DeInit(p_pwm->h_tim)== HAL_OK)
      {
        p_pwm->is_pwmInit=false;
      }
    if (HAL_TIM_Base_DeInit(p_pwm->h_tim)==HAL_OK)
      {
        p_pwm->is_timeInit=false;
      }

  return true;
}

bool pwmStop(uint8_t ch)
{
  if (ch >= PWM_MAX_CH) return false;
  pwm_t* p_pwm = &pwm_tbl[ch];

  HAL_TIM_PWM_Stop(p_pwm->h_tim, p_pwm->channel);

  return true;
}

//readout time, it must be be at least 3694 * 4
// Integrated time : SH must be at least 20

bool CCDset(uint32_t SH, uint32_t ICG)
{
  if (ICG <(3694*4) && SH<20) return false;

  /* Stop   */
  pwmStop(_DEF_PWM1); // ICG
  pwmStop(_DEF_PWM3); // SH
  current_SH = SH;
  current_ICG = ICG;
  /*re-configuration*/
  pwmBegin(_DEF_PWM1, ICG);
  pwmBegin(_DEF_PWM3, SH);

  /* Restart */
  pwmStart(_DEF_PWM1);
  pwmSycDelay(_DEF_PWM1, 1);
  pwmStart(_DEF_PWM3);
  return true;
}

void CCDSingleShot()
{
  pwmStart(_DEF_PWM1);
  pwmSycDelay(_DEF_PWM1, 1);
  pwmStart(_DEF_PWM3);
  ADCstart_DMA(_DEF_ADC1);

}


void pwmSycDelay(uint8_t ch, uint32_t delay)
{
  pwm_t* p_pwm = &pwm_tbl[ch];
  // 1 clock is delayed when using encapulated function. To synchronize, add 1 clock
  p_pwm->h_tim->Instance->CNT = delay+1;
}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
  if(tim_baseHandle->Instance==TIM2)
  {
    __HAL_RCC_TIM2_CLK_ENABLE();
  }
  else if(tim_baseHandle->Instance==TIM3)
  {
    __HAL_RCC_TIM3_CLK_ENABLE();
  }
  else if(tim_baseHandle->Instance==TIM4)
  {
    __HAL_RCC_TIM4_CLK_ENABLE();
  }
  else if(tim_baseHandle->Instance==TIM5)
  {
    __HAL_RCC_TIM5_CLK_ENABLE();
  }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timHandle->Instance==TIM2)
  {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM2 GPIO Configuration
    PA15     ------> TIM2_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
  else if(timHandle->Instance==TIM3)
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM3 GPIO Configuration
    PB4     ------> TIM3_CH1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
  else if(timHandle->Instance==TIM4)
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM4 GPIO Configuration
    PB8     ------> TIM4_CH3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }

}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
    __HAL_RCC_TIM2_CLK_DISABLE();
  }
  else if(tim_baseHandle->Instance==TIM3)
  {
    __HAL_RCC_TIM3_CLK_DISABLE();
  }
  else if(tim_baseHandle->Instance==TIM4)
  {
    __HAL_RCC_TIM4_CLK_DISABLE();
  }
  else if(tim_baseHandle->Instance==TIM5)
  {
    __HAL_RCC_TIM5_CLK_DISABLE();
  }
}


static void cliCCD(cli_args_t* args)
{
  bool ret = false;
  uint32_t SH;
  uint32_t ICG;
  if (args->argc==3 && args->isStr(0, "set")==true)
    {
      SH=args->getData(1);
      ICG=args->getData(2);
      if (CCDset(SH, ICG) !=true)
	{
	  cliPrintf("SH > 20 and ICG > 3694*4");
	}
      cliPrintf("SH : %d and ICG : %d", SH, ICG);
      ret = true;
    }

  if (args->argc==1 && args->isStr(0, "single")==true)
    {
      CCDSingleShot();
      cliPrintf("SingleShot");
      ret = true;
    }


  if(ret!=true)
    {
      cliPrintf("CCD set SH ICG\n");
      cliPrintf("CCD single\n");
    }
}

#endif
