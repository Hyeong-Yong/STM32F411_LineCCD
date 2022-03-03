/*
 * flash.c
 *
 *  Created on: 2022. 2. 10.
 *      Author: HYJH
 */

#ifndef SRC_HW_DRIVER_FLASH_C_
#define SRC_HW_DRIVER_FLASH_C_

#include "flash.h"


#ifdef _USE_HW_FLASH

#define FLASH_SECTOR_MAX    64  //stm32f1 => flash memory size 64kByte

typedef struct
{
  uint32_t addr;
  uint16_t length;
} flash_tbl_t;

static flash_tbl_t flash_tbl[FLASH_SECTOR_MAX];
static bool flashInSector(uint16_t sector_num, uint32_t addr, uint32_t length);

bool flashInit()
{
  for (int i=0; i<FLASH_SECTOR_MAX; i++)
    {
       flash_tbl[i].addr   =  0x8000000 + i*1024;
       flash_tbl[i].length =  1024;
    }
    return true;
}

bool flashInSector(uint16_t sector_num, uint32_t addr, uint32_t length)
{
  bool ret= false;

  uint32_t sector_start;
  uint32_t sector_end;
  uint32_t flash_start;
  uint32_t flash_end;

  sector_start  = flash_tbl[sector_num].addr;
  sector_end    = flash_tbl[sector_num].addr + flash_tbl[sector_num].length - 1;
  flash_start   = addr;
  flash_end     = addr + length - 1;

  if (sector_start >= flash_start && sector_start <= flash_end)
    {
      ret = true; //1
    }
  if (sector_end >= flash_start && sector_end <= flash_end)
    {
      ret = true;   //2  // 1,2 설명-> 지워야할 flash 메모리가 sector 보다 클 떄, sector 는 flash 양끝 안에 있어야한다.
    }
  if (flash_start >= sector_start && flash_start <= sector_end)
    {
      ret = true;
    }
  if (flash_end >= sector_start && flash_end <= sector_end)
    {
      ret = true;
    }
  return ret;
}

bool flashErase(uint32_t addr, uint32_t length) //flash를 지울때는 sector 단위로 지울수 있으며,
{
bool ret=false;

HAL_StatusTypeDef status;
FLASH_EraseInitTypeDef init;
uint32_t page_error;

int16_t start_sector_num = -1;
uint32_t sector_count = 0;

for (int i=0; i<FLASH_SECTOR_MAX; i++)
  {
    if (flashInSector(i, addr, length)==true)
      {
        if (start_sector_num < 0)
          {
            start_sector_num = i;
          }
        sector_count++;
      }
  }

if (sector_count > 0)
  {
    HAL_FLASH_Unlock();
init.TypeErase   = FLASH_TYPEERASE_PAGES;
init.Banks       = FLASH_BANK_1;
init.PageAddress = flash_tbl[start_sector_num].addr;
init.NbPages     = sector_count;
status = HAL_FLASHEx_Erase(&init, &page_error);
    if (status==HAL_OK)
      {
        ret = true;
      }
    HAL_FLASH_Lock();
  }

return ret;
}

bool flashWrite(uint32_t addr, uint8_t *p_data, uint32_t length) // 쓰고 읽을때는 바이트나 워드단위로 사용할수 있다
{
bool ret = true;

HAL_StatusTypeDef status;

if (addr%2 != 0)
  {
    return false; //16바이트가 최소단위이므로 얼라인된 데이터만 받을수 있게 설정, 짝수만 받는다
  }

HAL_FLASH_Unlock();

for (int i = 0 ; i<length ; i+=2)
  {
    uint16_t data;

    data  = p_data[i+0] << 0;
    data |= p_data[i+1] << 8;

    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr+i, (uint64_t)data);
    if (status!=HAL_OK)
      {
        ret = false;
        break;
      }
}
HAL_FLASH_Lock();

return ret;
}

bool flashRead(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret=true;

  uint8_t *p_byte = (uint8_t *)addr;

  for (int i=0; i<length; i++)
    {
      p_data[i]=p_byte[i];
    }

return ret;
}

#endif

#endif /* SRC_HW_DRIVER_FLASH_C_ */
