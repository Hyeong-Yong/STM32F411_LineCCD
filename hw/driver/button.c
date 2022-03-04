/*
 * button.c
 *
 *  Created on: 2022. 3. 3.
 *      Author: HYJH
 */

#include "button.h"
#include "cli.h"


#ifdef _USE_HW_BUTTON

#ifdef _USE_HW_CLI
static void cliButton(cli_args_t *args);
#endif


typedef struct
{
  GPIO_TypeDef 	*port;
  uint32_t 	 pin;
  GPIO_PinState  onstate;
} button_tbl_t;

button_tbl_t button_tbl[BUTTON_MAX_CH] =
{
    {GPIOA, GPIO_PIN_0, GPIO_PIN_RESET},
};


bool buttonInit(void)
{
  bool ret = true;

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;

  for (int i =0 ; i<BUTTON_MAX_CH; i++)
    {
      GPIO_InitStruct.Pin = button_tbl[i].pin;
      HAL_GPIO_Init(button_tbl[i].port, &GPIO_InitStruct);
    }
#ifdef _USE_HW_CLI
  cliAdd("button", cliButton);

#endif

  return ret;
}

bool buttonGetPressed(uint8_t ch)
{
    bool ret= false;

    if (ch >=BUTTON_MAX_CH)
      {
	return ret=false;
      }

    if ( HAL_GPIO_ReadPin(button_tbl[ch].port, button_tbl[ch].pin) == button_tbl[ch].onstate)
      {
	ret = true;
      }


    return ret;
}

#ifdef _USE_HW_CLI
void cliButton(cli_args_t *args)
{
  bool ret = false;

  if (args->argc ==1 && args->isStr(0, "Show")==true)
    {
      while (cliKeepLoop())
	{
	  for (int i=0; i<BUTTON_MAX_CH;i++)
	    {
	      cliPrintf("%d", buttonGetPressed(i));
	    }
	  cliPrintf("\n");
	  delay(100);
	}
      ret= true;
    }

  if (ret != true)
    {
      cliPrintf("button show\n");
    }

}
#endif


#endif

