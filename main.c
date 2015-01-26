/**
  ******************************************************************************
  * @file    ExtPlate/main.c 
  * @author  Kiselev.E
  * @version V1.0.0
  * @date    06-December-2014
  * @brief   Main program body
  ******************************************************************************
  */ 

#include "main.h"
#include "stm8s.h"
#include "i2c_comm.h"
#include "crc8.h"
#include "interboards_comm.h"
#include "analog_inputs.h"
#include "analog_outputs.h"
#include "board.h"
#include "stdio.h"
#include "string.h"
#define PUTCHAR_PROTOTYPE int putchar (int c)

EXT_BOARD_PACKET_T      i2c_Buf;
EXT_PIN_MODE_T          PinConfig[__EXT_PIN_COUNT];
EXT_PIN_STATE_T         PinState[__EXT_PIN_COUNT];
EXT_POWER_OUT_MODE_T    PowerOutConfig[__EXT_POWER_OUT_COUNT];
EXT_POWER_OUT_STATE_T   PowerOutState[__EXT_POWER_OUT_COUNT];

#define AI_CHANNELS     (10)
AI_CHANNEL_STATE_T      AIState[AI_CHANNELS];
uint8_t                 AIValue[AI_CHANNELS], AICnt, CurAdcCh;

uint8_t ConfigNum, StateNum;

struct{
  uint8_t NewConfig     :1,
          NewState      :1,
          bit2          :1,
          bit3          :1,
          bit4          :1,
          bit5          :1,
          bit6          :1,
          bit7          :1;
          
}Flag;
 

void sysclock_init();
void ports_init();
void uart_init();
void TIM1_init(void);
void TIM2_init(void);
void TIM3_init(void);
void ADC_init(void);
void parce_i2c_data(void);
void periph_update(void);
void form_i2c_data(void);
  
void main(void){
  ConfigNum = StateNum = 0;
  sysclock_init();
  ports_init();
  uart_init();
  //TIM1_init();
  //TIM2_init();
  //TIM3_init();
  ADC_init();
  I2C_init();
  printf("External Pins Plate v1.0.0\n\r");  
  
  
  enableInterrupts();
  
  while (1){
    parce_i2c_data();
    periph_update();
    form_i2c_data();
  }
}

void sysclock_init(void){
  /* Set HSI (High Speed Internal) oscillator prescaler to 1 */
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}

void ports_init(void){
  uint8_t i;
  for(i = 0; i<__EXT_PIN_COUNT; i++){
    GPIO_Init(ExtPinGPIOPort[i], ExtPinNum[i], GPIO_MODE_IN_FL_NO_IT);
  }
  for(i = 0; i<__EXT_POWER_OUT_COUNT; i++){
    GPIO_Init(ExtPowerOutGPIOPort[i], ExtPowerOutNum[i], GPIO_MODE_OUT_PP_LOW_FAST);
  }
}

void uart_init(void){
  UART2_DeInit();
  UART2_Init((uint32_t)256000, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, UART2_PARITY_NO,
             UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TX_ENABLE);
}
 
void ADC_init(void){
  ADC1_DeInit();
  ADC1_PrescalerConfig(ADC1_PRESSEL_FCPU_D3);
  ADC1->CR1 |= ADC1_CR1_ADON;
}

void TIM1_init(void){
  TIM1_DeInit();
  
  /* Time Base configuration */
  /*
  TIM1_Prescaler = 0 (f/(prescaler+1)
  TIM1_CounterMode = TIM1_COUNTERMODE_UP
  TIM1_Period = 255
  TIM1_RepetitionCounter = 0
  */

  TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, 255, 0);  
  TIM1_OC1Init(TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_DISABLE, TIM1_OUTPUTNSTATE_DISABLE,
               0, TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_LOW, TIM1_OCIDLESTATE_RESET,
               TIM1_OCNIDLESTATE_SET);
  TIM1_OC2Init(TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_DISABLE, TIM1_OUTPUTNSTATE_DISABLE,
               0, TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_LOW, TIM1_OCIDLESTATE_RESET,
               TIM1_OCNIDLESTATE_SET);
  TIM1_OC3Init(TIM1_OCMODE_PWM1, TIM1_OUTPUTSTATE_DISABLE, TIM1_OUTPUTNSTATE_DISABLE,
               0, TIM1_OCPOLARITY_HIGH, TIM1_OCNPOLARITY_LOW, TIM1_OCIDLESTATE_RESET,
               TIM1_OCNIDLESTATE_SET);
  TIM1_Cmd(ENABLE);
  TIM1_CtrlPWMOutputs(ENABLE);
}

void PinPWMEnable(uint8_t pin, uint8_t val){
  switch(pin){
   case EXT_PIN_11:
    /* Disable the Channel 1: Reset the CCE Bit */
    TIM1->CCER1 &= (uint8_t)(~( TIM1_CCER1_CC1E));
    /* Set the Output State */
    TIM1->CCER1 |= (uint8_t)(TIM1_OUTPUTNSTATE_ENABLE & TIM1_CCER1_CC1E);    
    /* Set the Pulse value */
    TIM1->CCR1H = (uint8_t)(val >> 8);
    TIM1->CCR1L = (uint8_t)(val);
    break;
    
   case EXT_PIN_12:
    /* Disable the Channel 1: Reset the CCE Bit */
    TIM1->CCER1 &= (uint8_t)(~( TIM1_CCER1_CC2E));
    /* Set the Output State */
    TIM1->CCER1 |= (uint8_t)(TIM1_OUTPUTNSTATE_ENABLE & TIM1_CCER1_CC2E);
    /* Set the Pulse value */
    TIM1->CCR2H = (uint8_t)(val >> 8);
    TIM1->CCR2L = (uint8_t)(val);
    break;
    
   case EXT_PIN_13:
    /* Disable the Channel 1: Reset the CCE Bit */
    TIM1->CCER2 &= (uint8_t)(~( TIM1_CCER2_CC3E));
    /* Set the Output State */
    TIM1->CCER2 |= (uint8_t)(TIM1_OUTPUTNSTATE_ENABLE & TIM1_CCER2_CC3E);
    /* Set the Pulse value */
    TIM1->CCR3H = (uint8_t)(val >> 8);
    TIM1->CCR3L = (uint8_t)(val);    
    break;
  }
}

void PinPWMDisable(uint8_t pin){
  switch(pin){
   case EXT_PIN_11:
    /* Disable the Channel 1: Reset the CCE Bit */
    TIM1->CCER1 &= (uint8_t)(~( TIM1_CCER1_CC1E));
    break;
    
   case EXT_PIN_12:
    /* Disable the Channel 1: Reset the CCE Bit */
    TIM1->CCER1 &= (uint8_t)(~( TIM1_CCER1_CC2E));
    break;
    
   case EXT_PIN_13:
    /* Disable the Channel 1: Reset the CCE Bit */
    TIM1->CCER2 &= (uint8_t)(~( TIM1_CCER2_CC3E));
    break;
  }
}

void PinPWMSetVal(uint8_t pin, uint8_t val){
  switch(pin){
   case EXT_PIN_11:
    /* Set the Pulse value */
    TIM1->CCR1H = (uint8_t)(val >> 8);
    TIM1->CCR1L = (uint8_t)(val);
    break;
   case EXT_PIN_12:
    /* Set the Pulse value */
    TIM1->CCR2H = (uint8_t)(val >> 8);
    TIM1->CCR2L = (uint8_t)(val);
    break;
   case EXT_PIN_13:
    /* Set the Pulse value */
    TIM1->CCR3H = (uint8_t)(val >> 8);
    TIM1->CCR3L = (uint8_t)(val);    
    break;
  }
}



void TIM2_init(void){
  TIM2_DeInit();
  TIM2_TimeBaseInit(TIM2_PRESCALER_1, 255);
  TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_DISABLE, 0, TIM2_OCPOLARITY_HIGH);  
  TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_DISABLE, 0, TIM2_OCPOLARITY_HIGH);
  TIM2_Cmd(ENABLE);
}

void TIM3_init(void){
  TIM3_DeInit();
  TIM3_TimeBaseInit(TIM3_PRESCALER_1, 255);
  TIM3_OC1Init(TIM3_OCMODE_PWM1, TIM3_OUTPUTSTATE_DISABLE, 0, TIM3_OCPOLARITY_HIGH);
  TIM3_OC2Init(TIM3_OCMODE_PWM1, TIM3_OUTPUTSTATE_DISABLE, 0, TIM3_OCPOLARITY_HIGH);
  TIM3_Cmd(ENABLE);
}

void PowerOutPWMEnable(uint8_t ch, uint8_t val){
  switch(ch){
   case EXT_POWER_OUT_0:
    /* Disable the Channel 1: Reset the CCE Bit, Set the Output State */
    TIM2->CCER1 &= (uint8_t)(~( TIM2_CCER1_CC1E));
    /* Set the Output State &  Set the Output Polarity  */
    TIM2->CCER1 |= (uint8_t)(TIM2_OUTPUTSTATE_ENABLE & TIM2_CCER1_CC1E );
    /* Set the Pulse value */
    TIM2->CCR1H = (uint8_t)(val >> 8);
    TIM2->CCR1L = (uint8_t)(val);
    break;
    
   case EXT_POWER_OUT_1:
    /* Disable the Channel 1: Reset the CCE Bit, Set the Output State */
    TIM2->CCER1 &= (uint8_t)(~( TIM2_CCER1_CC2E));
    /* Set the Output State &  Set the Output Polarity  */
    TIM2->CCER1 |= (uint8_t)(TIM2_OUTPUTSTATE_ENABLE & TIM2_CCER1_CC2E );
    /* Set the Pulse value */
    TIM2->CCR2H = (uint8_t)(val >> 8);
    TIM2->CCR2L = (uint8_t)(val);
    break;
    
   case EXT_POWER_OUT_2:
    /* Disable the Channel 1: Reset the CCE Bit, Set the Output State */
    TIM3->CCER1 &= (uint8_t)(~( TIM3_CCER1_CC1E));
    /* Set the Output State &  Set the Output Polarity  */
    TIM3->CCER1 |= (uint8_t)(TIM3_OUTPUTSTATE_ENABLE & TIM3_CCER1_CC1E );
    /* Set the Pulse value */
    TIM3->CCR1H = (uint8_t)(val >> 8);
    TIM3->CCR1L = (uint8_t)(val);
    break;
    
   case EXT_POWER_OUT_3:
    /* Disable the Channel 1: Reset the CCE Bit, Set the Output State */
    TIM3->CCER1 &= (uint8_t)(~( TIM3_CCER1_CC2E));
    /* Set the Output State &  Set the Output Polarity  */
    TIM3->CCER1 |= (uint8_t)(TIM3_OUTPUTSTATE_ENABLE & TIM3_CCER1_CC2E );
    /* Set the Pulse value */
    TIM3->CCR2H = (uint8_t)(val >> 8);
    TIM3->CCR2L = (uint8_t)(val);
    break;
  }
}

void PowerOutPWMDisable(uint8_t ch){
  switch(ch){
   case EXT_POWER_OUT_0:
    /* Disable the Channel 1: Reset the CCE Bit, Set the Output State */
    TIM2->CCER1 &= (uint8_t)(~( TIM2_CCER1_CC1E));
    break;
    
   case EXT_POWER_OUT_1:
    /* Disable the Channel 1: Reset the CCE Bit, Set the Output State */
    TIM2->CCER1 &= (uint8_t)(~( TIM2_CCER1_CC2E));
    break;
    
   case EXT_POWER_OUT_2:
    /* Disable the Channel 1: Reset the CCE Bit, Set the Output State */
    TIM3->CCER1 &= (uint8_t)(~( TIM3_CCER1_CC1E));
    break;
    
   case EXT_POWER_OUT_3:
    /* Disable the Channel 1: Reset the CCE Bit, Set the Output State */
    TIM3->CCER1 &= (uint8_t)(~( TIM3_CCER1_CC2E));
    break;
  }
}

void PowerOutPWMSetVal(uint8_t ch, uint8_t val){
  switch(ch){
   case EXT_POWER_OUT_0:
    /* Set the Pulse value */
    TIM2->CCR1H = (uint8_t)(val >> 8);
    TIM2->CCR1L = (uint8_t)(val);
    break;
    
   case EXT_POWER_OUT_1:
    /* Set the Pulse value */
    TIM2->CCR2H = (uint8_t)(val >> 8);
    TIM2->CCR2L = (uint8_t)(val);
    break;
    
   case EXT_POWER_OUT_2:
    /* Set the Pulse value */
    TIM3->CCR1H = (uint8_t)(val >> 8);
    TIM3->CCR1L = (uint8_t)(val);
    break;
    
   case EXT_POWER_OUT_3:
    /* Set the Pulse value */
    TIM3->CCR2H = (uint8_t)(val >> 8);
    TIM3->CCR2L = (uint8_t)(val);
    break;
  }
}

void print_packet(uint8_t * buf, uint8_t len){
  while(len--){
    printf("%d ", *buf++);
  }
}


void parce_i2c_data(void){
  uint8_t len;
  len = I2C_get_data(&i2c_Buf);
  if(len){
    printf("Packet received, len = %d\r\n", len);
    print_packet((uint8_t*)&i2c_Buf, len);
    if(crc8_comp((uint8_t*)&i2c_Buf, 0xFF, len) == 0){
      printf("CRC OK, code = %d\r\n", i2c_Buf.CmdCode);
      if(i2c_Buf.CmdCode == CMD_SETUP_CODE){
        ConfigNum = i2c_Buf.CmdSetup.ConfigNum;
        printf("   SETUP packet\r\n");
        memcpy(&PinConfig[0], &i2c_Buf.CmdSetup.PinConfig[0], __EXT_PIN_COUNT);
        memcpy(&PowerOutConfig[0], &i2c_Buf.CmdSetup.PowerOutConfig[0], __EXT_POWER_OUT_COUNT);
        Flag.NewConfig = 1;
      }
      
      if(i2c_Buf.CmdCode == CMD_PUSH_CODE){
        StateNum = i2c_Buf.CmdPush.StateNum;
        printf("   PUSH packet\r\n");
        memcpy(&PinState[0], &i2c_Buf.CmdPush.PinState[0], __EXT_PIN_COUNT);
        memcpy(&PowerOutState[0], &i2c_Buf.CmdPush.PowerOutState[0], __EXT_POWER_OUT_COUNT);
        Flag.NewState = 1;
      }
    } else {
      printf(" CRC ERROR\r\n");
    }
  }
}

void periph_config_correction(void){
  int8_t i, mistakes = 0;
  //printf("Peripheral config correction\r\n");
  for(i = 0; i < __EXT_PIN_COUNT; i++){
    // check for mistakes in config data
    switch(PinConfig[i]){
     case ANALOG_IN:
      if(!(CAN_BE_ANALOG_IN(i))){
        PinConfig[i] = INPUT_Z;   // if can't used as analog input, pin config as floating input
        //printf("   Pin %d can't be configured as Analog Input. Config as Digital Input - HiZ\r\n", i);
        mistakes++;
      }
      break;
      
     case ANALOG_OUT:
      if(!(CAN_BE_ANALOG_OUT(i))){
        PinConfig[i] = INPUT_Z; // if can't used as analog output, pin config as floating input
       // printf("   Pin %d can't be configured as Analog Output. Config as Digital Input - HiZ\r\n", i);
        mistakes++;
      }
      break;
      
     case OUTPUT_PP:
     case OUTPUT_OD:
      if(!CAN_BE_DIGITAL_OUT(i)){
        PinConfig[i] = INPUT_Z; // if can't used as digital output, pin config as floating input
        //printf("   Pin %d can't be configured as Digital Output. Config as Digital Input - HiZ\r\n", i);
        mistakes++;
      }
      break;
      
     case NOT_USED:
     case INPUT_PU:
     case INPUT_Z:
      break;
      
     default:
      //printf("   Pin %d unsupported config value: %d. Config as Digital Input - HiZ\r\n", i, PinConfig[i]);
      PinConfig[i] = INPUT_Z;   // if unsupported pin mode, pin config as floating input
      mistakes++;
      break;
    }
    GPIO_Init(ExtPinGPIOPort[i], ExtPinNum[i], ExtPinMode[PinConfig[i]]);
  }
  for(i = 0; i < __EXT_POWER_OUT_COUNT; i++){
    if(PowerOutConfig[i] != PWM){
      PowerOutConfig[i] = LOGICAL;
    }
    GPIO_Init(ExtPowerOutGPIOPort[i], ExtPowerOutNum[i], ExtPowerOutMode[PowerOutConfig[i]]);
  }
  //if(mistakes) printf("   Mistakes of config received: %d\r\n", mistakes);
  //printf("Peripheral config correction done!\r\n");
  //printf("\r\n");
}


void periph_config_apply(void){
  //printf("Peripheral config apply process\r\n");
  int8_t i; 
  uint8_t adcCh;
  AICnt = 0;
  for(i = 0; i < __EXT_PIN_COUNT; i++){    
    if(CAN_BE_ANALOG_OUT(i)){
      if(PinConfig[i] == ANALOG_OUT){
        //printf("   AO pin: %d\r\n", i);
        PinPWMEnable(i, PinState[i].AnalogOut);
      } else {
        PinPWMDisable(i);
      }
    }
    
    if(CAN_BE_DIGITAL_OUT(i)){
      if(IS_DIGITAL_OUTPUT(PinConfig[i])){
        if(PinState[i].DigitalOut){
          //printf("   DO pin: %d, write High\r\n", i);
          GPIO_WriteHigh(ExtPinGPIOPort[i], ExtPinNum[i]);
        } else {
          GPIO_WriteLow(ExtPinGPIOPort[i], ExtPinNum[i]);
          //printf("   DO pin: %d, write Low\r\n", i);
        }
      }
    }
    
    if(CAN_BE_ANALOG_IN(i)){
      adcCh = ExtPinAdcChannel((EXT_PIN_NAME_T)i);
      if(PinConfig[i] == ANALOG_IN){
        //printf("   AI pin: %d, ADC Channel:%d\r\n", i, adcCh);
        AIState[adcCh] = ENABLED;
        ADC1_SchmittTriggerConfig((ADC1_SchmittTrigg_TypeDef)adcCh, DISABLE);
        AICnt++;
      } else {
        AIState[adcCh] = DISABLED;
        ADC1_SchmittTriggerConfig((ADC1_SchmittTrigg_TypeDef)adcCh, ENABLE);
      }
    }
  }
  
  if(AICnt != 0){
    //printf("Starting AD conversion on %d pins. ", AICnt);
    for(i = 0; AIState[i] == DISABLED; i++);
    CurAdcCh = i;
    //printf("First conversion ADC channel is:%d \r\n", i);
    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, (ADC1_Channel_TypeDef)i, ADC1_ALIGN_LEFT);
    ADC1_ITConfig(ADC1_IT_EOCIE, ENABLE);
    ADC1_StartConversion();
  } else {
    //printf("Disable ADC converter, because of no analog inputs\r\n");
    ADC1_ITConfig(ADC1_IT_EOCIE, DISABLE);
  }
  
  for(i = 0; i < __EXT_POWER_OUT_COUNT; i++){
    if(PowerOutConfig[i] == PWM){
      //printf("Enable PWM on Power Out %d, with duty cycle %d%% \r\n", i, (PowerOutState[i].PWM)*100/256);
      PowerOutPWMEnable(i, PowerOutState[i].PWM);
    } else {
      PowerOutPWMDisable(i);
      if(PowerOutState[i].DigitalOut){
        //printf("PO pin: %d, write High\r\n", i);
        GPIO_WriteHigh(ExtPowerOutGPIOPort[i], ExtPowerOutNum[i]);
      } else {
        GPIO_WriteLow(ExtPowerOutGPIOPort[i], ExtPowerOutNum[i]);
        //printf("PO pin: %d, write Low\r\n", i);
      }
    }
  }
}

void periph_new_state(void){
  int8_t i; 
  for(i = 0; i < __EXT_PIN_COUNT; i++){
    switch(PinConfig[i]){
     case ANALOG_OUT:
      //printf("Pin %d, (analog out), set value: %dV\r\n", i, PinState[i].AnalogOut*5/256);
      PinPWMSetVal(i, PinState[i].AnalogOut);
      break;
      
     case OUTPUT_PP:
     case OUTPUT_OD:
      if(PinState[i].DigitalOut){
        //printf("Pin:%d, set High state\r\n", i);
        GPIO_WriteHigh(ExtPinGPIOPort[i], ExtPinNum[i]);
      } else {
        //printf("Pin:%d, set Low state\r\n", i);
        GPIO_WriteLow(ExtPinGPIOPort[i], ExtPinNum[i]);
      }
      break;
    }
  }
  for(i = 0; i < __EXT_POWER_OUT_COUNT; i++){
    switch(PowerOutConfig[i]){
     case LOGICAL:
      if(PowerOutState[i].DigitalOut == TRUE){
        //printf("Power Out:%d, (LOGICAL) set High state\r\n", i);
        GPIO_WriteHigh(ExtPowerOutGPIOPort[i], ExtPowerOutNum[i]);
      } else {
        //printf("Power Out:%d, (LOGICAL) set Low state\r\n", i);
        GPIO_WriteLow(ExtPowerOutGPIOPort[i], ExtPowerOutNum[i]);
      }
      break;
      
     case PWM:
      //printf("Power Out:%d, (PWM) set value:%d\r\n", i, PowerOutState[i].PWM);
      PowerOutPWMSetVal(i, PowerOutState[i].PWM);
      break;
    }
  }
}

void periph_get_state(void){
  uint8_t i;
  static uint8_t cnt = 0;
  cnt++;
  for(i = 0; i < __EXT_PIN_COUNT; i++){
    if(IS_DIGITAL_INPUT(PinConfig[i])){
      if(GPIO_ReadInputPin(ExtPinGPIOPort[i], ExtPinNum[i])){
        PinState[i].DigitalIn = TRUE;
      } else {
        PinState[i].DigitalIn = FALSE;
      }
      continue;
    }
    if(PinConfig[i] == ANALOG_IN){
      PinState[i].AnalogIn = AIValue[ExtPinAdcChannel((EXT_PIN_NAME_T)i)];
      if(cnt == 0){
        //printf("P%d=%d ", i, AIValue[ExtPinAdcChannel((EXT_PIN_NAME_T)i)]);
      }
    }
  }
  if((cnt == 0) & (AICnt != 0)) //printf("\r\n");
  if(AICnt != 0){
   ADC1_ITConfig(ADC1_IT_EOCIE, ENABLE);
   ADC1_StartConversion();
  }
}

void periph_update(void){
  if(Flag.NewConfig){
    Flag.NewConfig = 0;
    periph_config_correction();
    periph_config_apply();
  }
  if(Flag.NewState){
    Flag.NewState = 0;
    periph_new_state();
  }
  periph_get_state();
}


void form_i2c_data(void){
  static uint8_t cnt = 0;
  cnt++;
  i2c_Buf.CmdCode = CMD_POLL_CODE;
  i2c_Buf.CmdPoll.ConfigNum = ConfigNum;
  i2c_Buf.CmdPoll.StateNum = StateNum;
  memcpy(&i2c_Buf.CmdPoll.PinState[0], &PinState[0], sizeof(PinState));
  i2c_Buf.CmdPoll.CRC8 = crc8_comp((uint8_t*)&i2c_Buf, 0xFF, sizeof(i2c_Buf.CmdPoll) - 1);
  I2C_set_data(POLL, &i2c_Buf);
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
     if(AIState[i] == ENABLED){
       ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, (ADC1_Channel_TypeDef)i, ADC1_ALIGN_LEFT);
       CurAdcCh = i;
       break;
     }
   }
   ADC1_ClearITPendingBit(ADC1_IT_EOC);
 }

PUTCHAR_PROTOTYPE
{
  /* Write a character to the UART1 */
  UART2_SendData8(c);
  
  /* Loop until the end of transmission */
  while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET);

  return (c);
}


#ifdef USE_FULL_ASSERT
void assert_failed(u8* file, u32 line){  
  printf("Wrong parameters value: file %s on line %d \r\n", (char *)file,(char *)line);
  while (1);
}
#endif

