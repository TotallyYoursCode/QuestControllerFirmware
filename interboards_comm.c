#include "interboards_comm.h"
#include "crc8.h"
#include <string.h>

//static I2C_PACKET_T i2c_buf;


void i2c_clear_buf(EXT_BOARD_PACKET_T * buf){
  memset(&buf, 0, sizeof(buf));
}

void packet_crc8_compute(EXT_BOARD_PACKET_T * buf, uint8_t size){
  uint8_t crc8 = 0xFF;
  *((uint8_t*)buf+size) = crc8_comp((uint8_t*)buf, crc8, size);
}
       
void i2c_parse_buf(EXT_BOARD_PACKET_T * buf){
  uint8_t crc8;
  crc8 = 0xFF;
  switch(buf->CmdCode){
   case CMD_SETUP_CODE:
    if(crc8_comp((uint8_t*)buf, crc8, sizeof(buf->CmdSetup)) == 0){ // check CRC8
      
    } else {
      i2c_clear_buf(buf);
    }
    break;
    
   case CMD_PUSH_CODE:
    if(crc8_comp((uint8_t*)buf, crc8, sizeof(buf->CmdPush)) == 0){
    } else {
      i2c_clear_buf(buf);
    }
    break;
    
   default:
    i2c_clear_buf(buf);
    break;
  }
}

