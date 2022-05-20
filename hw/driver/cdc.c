/*
 * cdc.c
 *
 *  Created on: 2022. 1. 3.
 *      Author: HYJH
 */

#include "cdc.h"

#ifdef _USE_HW_CDC

static bool is_init = false;

bool cdcInit(void)
{
  bool ret =true;

  is_init = true;

  return ret;
}

bool cdcIsInit()
{
  return is_init;
}

#endif

//cdc function은 usbd_cdc_if.c 에 있음.
