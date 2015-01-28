#ifndef _ANALOG_OUTPUTS_H_
#define _ANALOG_OUTPUTS_H_

#include "stm8s.h"
#include "stdio.h"
#include "interboards_comm.h"

#define ANALOG_OUT_AUTORELOAD			(255)

// The counter clock frequency fCK_CNT is equal to fCK_PSC/ (PSCR+1)
#define ANALOG_OUT_FREQ_PRESCALER		(0)

// The counter clock frequency fCK_CNT is equal to fCK_PSC/ 2^(PSCR)
#define POWER_OUT_PWM_FREQ_PRESCALER 	(10)

void analogOutputsInit(void);
void analogOutputEnable(EXT_PIN_NAME_T pin, uint8_t value);
void analogOutputDisable(EXT_PIN_NAME_T pin);
void powerOutPWMEnable(EXT_POWER_OUT_NAME_T pin, uint8_t value);
void powerOutPWMDisable(EXT_POWER_OUT_NAME_T pin);

#endif