#ifndef _I2C_COMM_H_
#define _I2C_COMM_H_

#include "stm8s_it.h"
#include "interboards_comm.h"
 
typedef enum{
  POLL,
  _TRANSMIT_PACKET_NUM
} TRANSMIT_PACKET_TYPE;


void I2C_init(void);
uint8_t I2C_get_data(EXT_BOARD_PACKET_T * buf);
uint8_t I2C_set_data(TRANSMIT_PACKET_TYPE cmd, EXT_BOARD_PACKET_T * buf);


#endif