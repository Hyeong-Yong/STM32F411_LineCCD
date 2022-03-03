/*
 * qbuffer.h
 *
 *  Created on: 2022. 1. 3.
 *      Author: HYJH
 */

#ifndef SRC_COMMON_HW_INCLUDE_QBUFFER_H_
#define SRC_COMMON_HW_INCLUDE_QBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"

  typedef struct
  {
    uint32_t head;
    uint32_t tail;
    uint32_t len;
    uint8_t *p_buf;
  } qbuffer_t; //구조체에서 p_buf는 구조체에서 정의하지 않고 사용하는쪽에서 동적 할당해서 유동적으로 사용할수 있겠금 설정.

  void     qbufferInit(void);
  bool     qbufferCreate(qbuffer_t *p_node, uint8_t *p_buf, uint32_t length);
  bool     qbufferWrite(qbuffer_t *p_node, uint8_t *p_data, uint32_t length);
  bool     qbufferRead(qbuffer_t *p_node, uint8_t *p_data, uint32_t length);
  uint32_t qbufferAvailable(qbuffer_t *p_node);
  void     qbufferFlush(qbuffer_t *p_node); //in =out 일때 비워진다.

#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_HW_INCLUDE_QBUFFER_H_ */
