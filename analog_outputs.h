#ifndef _ANALOG_OUTPUTS_H_
#define _ANALOG_OUTPUTS_H_

#include "stm8s.h"
#include "stdio.h"
#include "interboards_comm.h"

#define ANALOG_OUT_AUTORELOAD		(255)
#define ANALOG_OUT_FREQ_PRESCALER	(1)

void analogOutputsInit(void);
void analogOutputEnable(EXT_PIN_NAME_T pin, uint8_t value);
void analogOutputDisable(EXT_PIN_NAME_T pin);

#endif