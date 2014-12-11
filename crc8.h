#ifndef _CRC8_H_
#define _CRC8_H_

#ifdef AVR_MCU
#include <stdint.h>
#elif defined(STM8S105)
#include "stm8s.h"
#elif defined(STM8L15X_MD)
#include "stm8l15x.h"
#endif

/* Undef this line if you want direct CRC8 computing
    otherwise it will be table driven */
//#define DIRECT_COMPUTING

#ifdef DIRECT_COMPUTING
#define crc8_comp       crc8_sft_buf
#else
#define crc8_comp       crc8_tbl_buf
#endif

#define poly8	0x31

/* Direct */
uint8_t crc8_sft_buf(uint8_t * buf, uint8_t crc, uint32_t len);
/* Table driven */
uint8_t crc8_tbl_buf(uint8_t * buf, uint8_t crc, uint32_t len);


#endif