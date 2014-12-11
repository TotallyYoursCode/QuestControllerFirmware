#ifndef _EXT_PINS_H_
#define _EXT_PINS_H_

#include "Arduino.h"
#include <Wire.h>
#include "interboards_comm.h"

/* Undef this line if you want direct CRC8 computing
    otherwise it will be table driven */
#define DIRECT_COMPUTING
#define poly8	0x31

class Ext{
  public:
   Ext(){
     memset(&buf, 0, sizeof(buf));
     memset(&PinConfig, 0, sizeof(PinConfig));
     memset(&PinState, 0, sizeof(PinState));
     memset(&PowerOutConfig, 0, sizeof(PowerOutConfig));
     memset(&PowerOutState, 0, sizeof(PowerOutState));
     PinConfigChanged = false;
     PinStateChanged = false;
     WireInitialized = false;
   }
    void loop(void);
    
    void PinMode(EXT_PIN_NAME_T pin, EXT_PIN_MODE_T mode);
    void DigitalWrite(EXT_PIN_NAME_T pin, bool state);
    bool DigitalRead(EXT_PIN_NAME_T pin);
    void AnalogWrite(EXT_PIN_NAME_T pin, uint8_t value);
    uint8_t AnalogRead(EXT_PIN_NAME_T pin);
    
    void PowerOutMode(EXT_POWER_OUT_NAME_T pin, EXT_POWER_OUT_MODE_T mode);
    void PowerOutWrite(EXT_POWER_OUT_NAME_T pin, uint8_t value);
    
  private:
    EXT_BOARD_PACKET_T          buf;
    EXT_PIN_MODE_T              PinConfig[__EXT_PIN_COUNT];
    EXT_PIN_STATE_T             PinState[__EXT_PIN_COUNT];
    bool                        PinConfigChanged;
    bool                        PinStateChanged;
    
    EXT_POWER_OUT_MODE_T        PowerOutConfig[__EXT_POWER_OUT_COUNT];
    EXT_POWER_OUT_STATE_T       PowerOutState[__EXT_POWER_OUT_COUNT];
    bool                        PowerOutConfigChanged;
    bool                        PowerOutStateChanged;
    
    bool                        WireInitialized;
    
    void write_i2c(uint8_t * buf, uint8_t size);
    void read_i2c(uint8_t * buf, uint8_t size);
    void buf_crc8_compute(uint8_t size);
    uint8_t crc8_comp(uint8_t * buf, uint8_t crc, uint32_t len);
};

#endif