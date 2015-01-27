#include "analog_outputs.h"

void analogOutputsInit(void)
{
	TIM1_DeInit();
	TIM1->CR1 |= TIM1_CR1_ARPE;	//auto-reload preload enable
	TIM1->ARRH  = (ANALOG_OUT_AUTORELOAD >> 8) & 0xFF; 		// set auto-reload register
	TIM1->ARRL  = (ANALOG_OUT_AUTORELOAD) & 0xFF;
	TIM1->PSCRH = (ANALOG_OUT_FREQ_PRESCALER >> 8) & 0xFF;	// set timer prescaler
	TIM1->PSCRL = (ANALOG_OUT_FREQ_PRESCALER) & 0xFF;
	TIM1->CCMR1 = 0;
	TIM1->CCMR1 = TIM1->CCMR2 = TIM1->CCMR3 = 	TIM1_OCMODE_PWM1 | 	//PWM mode 1.
	  											TIM1_CCMR_OCxPE;		//output compare preload enable
	// TIM1->CR1 |= TIM1_CR1_CEN;
}

void analogOutputEnable(EXT_PIN_NAME_T pin, uint8_t value)
{
	TIM1->CR1 &= (~TIM1_CR1_CEN);		// disable timer	
	switch (pin) {
	case EXT_PIN_11:
		TIM1->CCR1H = 0x00;				// set PWM value to ch 1
  		TIM1->CCR1L = value;
  		TIM1->CCER1 |= TIM1_CCER1_CC1E; // compare 1 output enable
		break;
		
	case EXT_PIN_12:
		TIM1->CCR2H = 0x00;				// set PWM value to ch 2
		TIM1->CCR2L = value;
		TIM1->CCER1 |= TIM1_CCER1_CC2E;	// compare 2 output enable
		break;
		
	case EXT_PIN_13:
		TIM1->CCR3H = 0x00;				// set PWM value to ch 1
		TIM1->CCR3L = value;
		TIM1->CCER2 |= TIM1_CCER2_CC3E;	// compare 3 output enable
		break;
	}
	TIM1->CR1 |= (TIM1_CR1_CEN);		// enable timer
}

void analogOutputDisable(EXT_PIN_NAME_T pin)
{
	TIM1->CR1 &= (~TIM1_CR1_CEN);		// disable timer
	switch (pin) {
	case EXT_PIN_11:
		TIM1->CCR1H = TIM1->CCR1L = 0x00;	// clear compare value
		TIM1->CCER1	&= (~TIM1_CCER1_CC1E);	// disable comp output 1
		break;
		
	case EXT_PIN_12:
		TIM1->CCR2H = TIM1->CCR2L = 0x00;	// clear compare value
		TIM1->CCER1 &= (~TIM1_CCER1_CC2E);	// disable comp output 2
		break;
		
	case EXT_PIN_13:
		TIM1->CCR3H = TIM1->CCR3L = 0x00;	// clear compare value
		TIM1->CCER2 &= (~TIM1_CCER2_CC3E);	// disable comp output 2
		break;
	}
	TIM1->CR1 |= (TIM1_CR1_CEN);		// enable timer	
}