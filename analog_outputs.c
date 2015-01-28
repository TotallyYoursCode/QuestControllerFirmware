#include "analog_outputs.h"

#include <stdio.h>

void analogOutputsInit(void)
{
	TIM1_DeInit();
	TIM1->CR1  |= TIM1_CR1_ARPE;	//auto-reload preload enable
	TIM1->ARRH  = (ANALOG_OUT_AUTORELOAD >> 8) & 0xFF; 		// set auto-reload register
	TIM1->ARRL  = (ANALOG_OUT_AUTORELOAD) & 0xFF;
	TIM1->PSCRH = (ANALOG_OUT_FREQ_PRESCALER >> 8) & 0xFF;	// set timer prescaler
	TIM1->PSCRL = (ANALOG_OUT_FREQ_PRESCALER) & 0xFF;
	TIM1->CCMR1 = TIM1->CCMR2 = TIM1->CCMR3 = 	TIM1_OCMODE_PWM1 | 	//PWM mode 1.
	  											TIM1_CCMR_OCxPE;	//output compare preload enable
	TIM1->BKR |= TIM1_BKR_MOE;			// enable PWM outputs
	TIM1->CR1 |= TIM1_CR1_CEN;			// enable timer 1
	
	TIM2_DeInit();
	TIM2->CR1  |= TIM2_CR1_ARPE;
	TIM2->ARRH  = (ANALOG_OUT_AUTORELOAD >> 8) & 0xFF; 		// set auto-reload register
	TIM2->ARRL  = (ANALOG_OUT_AUTORELOAD) & 0xFF;
	TIM2->PSCR  = (POWER_OUT_PWM_FREQ_PRESCALER) & 0xFF;	// set timer prescaler
	TIM2->CCMR1 = TIM2->CCMR2 = TIM2_OCMODE_PWM1 |	// PWM mode 1
								TIM2_CCMR_OCxPE;	// output compare preload enable
	TIM2->CR1  |= TIM2_CR1_CEN;
	
	TIM3_DeInit();
	TIM3->CR1  |= TIM3_CR1_ARPE;
	TIM3->ARRH  = (ANALOG_OUT_AUTORELOAD >> 8) & 0xFF; 		// set auto-reload register
	TIM3->ARRL  = (ANALOG_OUT_AUTORELOAD) & 0xFF;
	TIM3->PSCR  = (POWER_OUT_PWM_FREQ_PRESCALER) & 0xFF;	// set timer prescaler
	TIM3->CCMR1 = TIM3->CCMR2 = TIM3_OCMODE_PWM1 | // PWM mode 1.
								TIM3_CCMR_OCxPE;
	TIM3->CR1  |= TIM3_CR1_CEN;
}

void analogOutputEnable(EXT_PIN_NAME_T pin, uint8_t value)
{
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
}

void analogOutputDisable(EXT_PIN_NAME_T pin)
{
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
}

void powerOutPWMEnable(EXT_POWER_OUT_NAME_T pin, uint8_t value)
{
	switch (pin) {
	case EXT_POWER_OUT_0:
		TIM2->CCR1H  = 0x00;			// set PWM value to ch1
		TIM2->CCR1L  = value;
		TIM2->CCER1 |= TIM2_CCER1_CC1E; // enable 1 output compare
		break;
		
	case EXT_POWER_OUT_1:
		TIM2->CCR2H  = 0x00;			// set PWM value to ch2
		TIM2->CCR2L  = value;
		TIM2->CCER1 |= TIM2_CCER1_CC2E; // enable 1 output compare
		break;
	
	case EXT_POWER_OUT_2:
		TIM3->CCR1H  = 0x00;			// set PWM value to ch1
		TIM3->CCR1L  = value;
		TIM3->CCER1 |= TIM3_CCER1_CC1E; // enable 1 output compare
		break;
		
	case EXT_POWER_OUT_3:
		TIM3->CCR2H  = 0x00;			// set PWM value to ch2
		TIM3->CCR2L  = value;
		TIM3->CCER1 |= TIM3_CCER1_CC2E; // enable 1 output compare
		break;
	}		
}

void powerOutPWMDisable(EXT_POWER_OUT_NAME_T pin)
{
	switch (pin) {
	case EXT_POWER_OUT_0:
		TIM2->CCR1H  = TIM2->CCR1L = 0x00;	// clear PWM value to ch1
		TIM2->CCER1 &= (~TIM2_CCER1_CC1E); 	// disable 1 output compare
		break;
		
	case EXT_POWER_OUT_1:
		TIM2->CCR2H  = TIM2->CCR2L = 0x00;	// clear PWM value to ch2
		TIM2->CCER1 &= (~TIM2_CCER1_CC2E); 	// disable 1 output compare
		break;
	
	case EXT_POWER_OUT_2:
		TIM3->CCR1H  = TIM3->CCR1L = 0x00;  // clear PWM value to ch1
		TIM3->CCER1 &= (~TIM3_CCER1_CC1E);  // disable 1 output compare
		break;
		
	case EXT_POWER_OUT_3:
		TIM3->CCR2H  = TIM3->CCR2L = 0x00;	// clear PWM value to ch2
		TIM3->CCER1 &= (~TIM3_CCER1_CC2E); 	// disable 1 output compare
		break;
	}
}