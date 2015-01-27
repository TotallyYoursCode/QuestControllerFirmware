#ifndef _BOARD_H_
#define _BOARD_H_

#include "stm8s.h"
#include "interboards_comm.h"

/* MCU Port definitions for external pins */
extern GPIO_TypeDef * ExtPinGPIOPort[__EXT_PIN_COUNT];

/* MCU Pin number definitions for external pins */
extern GPIO_Pin_TypeDef ExtPinNum[__EXT_PIN_COUNT];

/* MCU Port definitions for external power outs */
extern GPIO_TypeDef * ExtPowerOutGPIOPort[__EXT_POWER_OUT_COUNT];

/* MCU Pin number definitions for external power outs */
extern GPIO_Pin_TypeDef ExtPowerOutNum[__EXT_POWER_OUT_COUNT];

/* pin config definition for all extrenal pins modes */
extern GPIO_Mode_TypeDef ExtPinMode[__EXT_PIN_MODE_COUNT];

/* pin config definition for all extrenal power output modes */
extern GPIO_Mode_TypeDef ExtPowerOutMode[__EXT_POWER_OUT_MODE_COUNT];

#define ADC_NOT_SUPPORTED  ((ADC1_Channel_TypeDef)(255))
ADC1_Channel_TypeDef ExtPinAdcChannel(EXT_PIN_NAME_T pin);

#endif