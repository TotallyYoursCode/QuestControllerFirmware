
#include "analog_inputs.h"
#include "board.h"

#define AI_CHANNELS     (10)
FunctionalState	AIState[AI_CHANNELS];
uint8_t			AIValue[AI_CHANNELS], CurAdcCh;
#define AVERAGE_CYCLES	(16)  // must be less than 64. see details in ADC Interrupt handler

void analogInputsInit(void)
{
  ADC1_DeInit();
  ADC1_PrescalerConfig(ADC1_PRESSEL_FCPU_D3);
  // ADC1->CR1 |= ADC1_CR1_ADON;
}

void analogInputEnable(EXT_PIN_NAME_T pin)
{
	uint8_t adcCh = ExtPinAdcChannel(pin);
	AIState[adcCh] = ENABLE;
    ADC1_SchmittTriggerConfig((ADC1_SchmittTrigg_TypeDef)adcCh, DISABLE);
	ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, (ADC1_Channel_TypeDef)adcCh, ADC1_ALIGN_RIGHT);
	CurAdcCh = adcCh;
}

void analogInputDisable(EXT_PIN_NAME_T pin)
{
	uint8_t adcCh = ExtPinAdcChannel(pin);
	AIState[adcCh] = DISABLE;
    ADC1_SchmittTriggerConfig((ADC1_SchmittTrigg_TypeDef)adcCh, ENABLE);
}


uint8_t analogInputGetValue(EXT_PIN_NAME_T pin)
{
	return AIValue[ExtPinAdcChannel(pin)];
}

void analogInputStartConversion(void)
{
	uint8_t i;
	for (i = 0; i < AI_CHANNELS; i++) {
		if (AIState[i] == ENABLE) {
			ADC1->CR1 |= ADC1_CR1_ADON;				// ADC switch on
			ADC1_ITConfig(ADC1_IT_EOCIE, ENABLE);	// enable ADC interrupt
			ADC1_StartConversion();					// start ADC conversion
			return;
		}
	}
	ADC1->CR1 &= (~ADC1_CR1_ADON);			// ADC switch off
	ADC1_ITConfig(ADC1_IT_EOCIE, DISABLE);	// disable ADC interrupt
}


/**
  * @brief ADC1 interrupt routine.
  * @par Parameters:
  * None
  * @retval
  * None
  */
 INTERRUPT_HANDLER(ADC1_IRQHandler, 22)
 {
	 static uint8_t averageCounter = 0;
	 static uint16_t ConvVal;
	 uint8_t i;
   
   ConvVal += ADC1_GetConversionValue();
   if (++averageCounter >= AVERAGE_CYCLES) {
	   AIValue[CurAdcCh] = ConvVal/AVERAGE_CYCLES/4;  // div by 4 because of 10-bit result
	   ConvVal = 0;
	   for (i = CurAdcCh + 1; i != CurAdcCh; i++) {
		   if (i >= AI_CHANNELS) {
			   i = 0;
		   }
		   if (AIState[i] == ENABLE) {
			   ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, (ADC1_Channel_TypeDef)i, ADC1_ALIGN_RIGHT);
			   CurAdcCh = i;
			   break;
		   }
	   }
   }
   ADC1_ClearITPendingBit(ADC1_IT_EOC);
 }
