#ifndef _ANALOG_INPUTS_H_
#define _ANALOG_INPUTS_H_

#include "interboards_comm.h"

void analogInputsInit(void);
void analogInputEnable(EXT_PIN_NAME_T pin);
void analogInputDisable(EXT_PIN_NAME_T pin);
void analogInputReconfigurate(void);
uint8_t analogInputGetValue(EXT_PIN_NAME_T pin);
void analogInputStartConversion(void);


#endif