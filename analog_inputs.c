
#include "analog_inputs.h"
#include "board.h"

#define AI_CHANNELS     (10)
FunctionalState	AIState[AI_CHANNELS];
uint8_t			AIValue[AI_CHANNELS], AICnt, CurAdcCh;

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
    AICnt++;
}

void analogInputDisable(EXT_PIN_NAME_T pin)
{
	uint8_t adcCh = ExtPinAdcChannel(pin);
	AIState[adcCh] = DISABLE;
    ADC1_SchmittTriggerConfig((ADC1_SchmittTrigg_TypeDef)adcCh, ENABLE);
}

void analogInputReconfigurate(void)
{
	uint8_t i;
	if (AICnt != 0) {
		//printf("Starting AD conversion on %d pins. ", AICnt);
    	for (i = 0; AIState[i] == DISABLE; i++);
    	CurAdcCh = i;
    	//printf("First conversion ADC channel is:%d \r\n", i);
    	ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, (ADC1_Channel_TypeDef)i, ADC1_ALIGN_LEFT);
		ADC1->CR1 |= ADC1_CR1_ADON;
    	ADC1_ITConfig(ADC1_IT_EOCIE, ENABLE);
    	ADC1_StartConversion();
	} else {
    	//printf("Disable ADC converter, because of no analog inputs\r\n");
		ADC1->CR1 &= (~ADC1_CR1_ADON);
    	ADC1_ITConfig(ADC1_IT_EOCIE, DISABLE);
	}
	AICnt = 0;
}

uint8_t analogInputGetValue(EXT_PIN_NAME_T pin)
{
	return AIValue[ExtPinAdcChannel(pin)];
}

void analogInputStartConversion(void)
{
  if(AICnt != 0){
   ADC1_ITConfig(ADC1_IT_EOCIE, ENABLE);
   ADC1_StartConversion();
  }
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
   uint16_t ConvVal;
   uint8_t i;
   ConvVal = ADC1_GetConversionValue();
   AIValue[CurAdcCh] = (ConvVal>>8)&0xFF;
   for(i = CurAdcCh + 1; i != CurAdcCh; i++){
     if(i>AI_CHANNELS)
       i = 0;
     if(AIState[i] == ENABLE){
       ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, (ADC1_Channel_TypeDef)i, ADC1_ALIGN_LEFT);
       CurAdcCh = i;
       break;
     }
   }
   ADC1_ClearITPendingBit(ADC1_IT_EOC);
 }
