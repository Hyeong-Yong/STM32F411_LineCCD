/*
 * qbuffer.c
 *
 *  Created on: 2022. 1. 3.
 *      Author: HYJH
 */

#include "qbuffer.h"

  void qbufferInit(void)
  {

  }


  bool qbufferCreate(qbuffer_t *p_node, uint8_t *p_buf, uint32_t length)
  {
    bool ret= true;
    p_node->head=0;
    p_node->tail=0;
    p_node->len=length;
    p_node->p_buf=p_buf;
    return ret;
  }

  bool qbufferWrite(qbuffer_t *p_node, uint8_t *p_data, uint32_t length)
  {
    bool ret = true;

    uint32_t next_head;

    for (int i=0; i<length ; i++)
      {
       next_head = (p_node->head + 1) % p_node -> len;

       if (next_head != p_node->tail) //쓸수 있는 공간이 비어 있으면, 다른말로 즉 [p_node-> head = 512, next_head =0, p_node->tail = 0]이면 쓸수 없음, 즉 버퍼 꽉참
         {
           if(p_node -> p_buf != NULL)
             {
               p_node->p_buf[p_node->head] = p_data[i];
             }
           p_node->head=next_head;
         }
       else // 버퍼 꽉참
         {
           ret=false;
           break;
         }
      }

    return ret;
  }


  bool qbufferRead(qbuffer_t *p_node, uint8_t *p_data, uint32_t length)
  {
    bool ret= true;

    for (int i=0; i<length ; i++)
      {
        if (p_node->p_buf != NULL)
          {
            p_data[i]=p_node->p_buf[p_node->tail];
          }
        if (p_node->tail != p_node->head)
          {
            p_node->tail = (p_node->tail+1) % p_node->len;
          }
        else  //p_node->out = p_node-> in 는 데이터가 안들어와서 없다는 말
          {
            ret=false;
            break;
          }
      }
    return ret;
  }


  uint32_t qbufferAvailable(qbuffer_t *p_node)
  {
    uint32_t ret;
    ret= (p_node->head - p_node->tail) % p_node->len;
    return ret;
  }

  void qbufferFlush(qbuffer_t *p_node)
  {
    p_node->head  = 0;
    p_node->tail = 0;
  }
