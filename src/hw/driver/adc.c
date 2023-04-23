/*
 * adc.c
 *
 *  Created on: May 24, 2022
 *      Author: HYJH
 */

#include "adc.h"
#include "cdc.h"
#include "usbd_cdc_if.h"
#ifdef _USE_HW_ADC

#define      ADC_BUF_SIZE	3648

typedef struct
{
  bool                    is_init;
  ADC_HandleTypeDef*	  hADCx;
  uint32_t                adc_channel;
} adc_tbl_t;


static adc_tbl_t adc_tbl[ADC_MAX_CH];

static volatile uint16_t adc_buf[ADC_BUF_SIZE];

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

void adcInit()
{

  for (int i = 0; i < ADC_MAX_CH; i++)
    {
      adc_tbl[i].is_init = false;

      ADC_ChannelConfTypeDef sConfig ={ 0 };

      /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
       */

      hadc1.Instance = ADC1;
      hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
      hadc1.Init.Resolution = ADC_RESOLUTION_12B;
      hadc1.Init.ScanConvMode = DISABLE;
      hadc1.Init.ContinuousConvMode = DISABLE;
      hadc1.Init.DiscontinuousConvMode = DISABLE;
      hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
      hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T5_CC3;
      hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
      hadc1.Init.NbrOfConversion = 1;
      hadc1.Init.DMAContinuousRequests = ENABLE;
      hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
      if (HAL_ADC_Init (&hadc1) == HAL_OK)
	{
	  adc_tbl[i].is_init = true;
	}
      /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
       */
      sConfig.Channel = ADC_CHANNEL_5;
      sConfig.Rank = 1;
      sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
      if (HAL_ADC_ConfigChannel (&hadc1, &sConfig) != HAL_OK)
	{
	  Error_Handler ();
	}

      adc_tbl[i].is_init     = true;
      adc_tbl[i].hADCx       = &hadc1;
      adc_tbl[i].adc_channel = ADC_CHANNEL_5;

    }

}

void ADCstart_DMA(uint8_t ch)
{
  HAL_ADC_Start_DMA (adc_tbl[ch].hADCx, (uint32_t*)&adc_buf, ADC_BUF_SIZE);
}

void ADCstop_DMA(uint8_t ch)
{
  HAL_ADC_Stop_DMA (adc_tbl[ch].hADCx);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
      uint8_t transfer_buf[4+(ADC_BUF_SIZE*2)];

      //start bits[2bit]
      transfer_buf[0]=0xFA;
      transfer_buf[1]=0xFB;
      //data bits [ADC_BUF_SIZE *2]
      for (int i =0 ; i< ADC_BUF_SIZE;i++)
	{
	  uint8_t bit_convert[2];
	  bit_convert[0]= adc_buf[i] >>0;
	  bit_convert[1]= adc_buf[i] >>8;
	  transfer_buf[2+i*2] = bit_convert[0];
	  transfer_buf[3+i*2] = bit_convert[1];
	}

      //end bits [2bit]
      transfer_buf[2+(ADC_BUF_SIZE*2)] = 0xEB;
      transfer_buf[3+(ADC_BUF_SIZE*2)] = 0xEA;

      CDC_Transmit_FS( (uint8_t*)transfer_buf, 4+(ADC_BUF_SIZE*2));

      //stop PWM and ADC
      pwmStop(_DEF_PWM1); // ICG
      pwmStop(_DEF_PWM3); // SH
      ADCstop_DMA(_DEF_ADC1);

  //crc?
  //end bit [4bit]
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

      /* DMA controller clock enable */
      __HAL_RCC_DMA2_CLK_ENABLE();

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA5     ------> ADC1_IN5
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ADC1 DMA Init */
    /* ADC1 Init */
    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc1);

    /* DMA interrupt init */
    /* DMA2_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

  }
}


void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA5     ------> ADC1_IN5
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5);

    /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);

    /* ADC1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(ADC_IRQn);
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}


#endif
