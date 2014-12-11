#include "board.h"
/* MCU Port definitions for external pins */
GPIO_TypeDef * ExtPinGPIOPort[__EXT_PIN_COUNT] = {
  /* EXT_PIN_0 : EXT_PIN_6 GPIO Ports */
  GPIOA, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB,  
  /* EXT_PIN_7 : EXT_PIN_13 GPIO Ports */
  GPIOB, GPIOB, GPIOE, GPIOE, GPIOC, GPIOC, GPIOC,  
  /* EXT_PIN_14 : EXT_PIN_20 GPIO Ports */
  GPIOC, GPIOC, GPIOC, GPIOG, GPIOG, GPIOE, GPIOE
};

/* MCU Pin number definitions for external pins */
GPIO_Pin_TypeDef ExtPinNum[__EXT_PIN_COUNT] = {
  /* EXT_PIN_0 : EXT_PIN_6 MCU pin pumbers */
  GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_5, GPIO_PIN_4, GPIO_PIN_3, GPIO_PIN_2,
  /* EXT_PIN_7 : EXT_PIN_13 MCU pin pumbers */
  GPIO_PIN_1, GPIO_PIN_0, GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
  /* EXT_PIN_14 : EXT_PIN_20 MCU pin pumbers */
  GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_3, GPIO_PIN_0
};

/* MCU Port definitions for external power outs */
GPIO_TypeDef * ExtPowerOutGPIOPort[__EXT_POWER_OUT_COUNT] = {
  GPIOD, GPIOD, GPIOD, GPIOD
};

/* MCU Pin number definitions for external power outs */
GPIO_Pin_TypeDef ExtPowerOutNum[__EXT_POWER_OUT_COUNT] = {
  GPIO_PIN_4, GPIO_PIN_3, GPIO_PIN_2, GPIO_PIN_0
};

/* pin config definition for all extrenal pins modes */
GPIO_Mode_TypeDef ExtPinMode[__EXT_PIN_MODE_COUNT] = {
  GPIO_MODE_IN_FL_NO_IT,        /* NOT_USED */
  GPIO_MODE_OUT_PP_LOW_FAST,    /* OUTPUT_PP */
  GPIO_MODE_OUT_OD_LOW_FAST,    /* OUTPUT_OD */
  GPIO_MODE_IN_PU_NO_IT,        /* INPUT_PU */
  GPIO_MODE_IN_FL_NO_IT,        /* INPUT_Z */
  GPIO_MODE_IN_FL_NO_IT,        /* ANALOG_IN */
  GPIO_MODE_OUT_PP_LOW_FAST     /* ANALOG_OUT */
};

/* pin config definition for all extrenal power output modes */
GPIO_Mode_TypeDef ExtPowerOutMode[__EXT_POWER_OUT_MODE_COUNT] = {
  GPIO_MODE_OUT_PP_LOW_FAST,    /* LOGICAL */
  GPIO_MODE_OUT_PP_LOW_FAST     /* PWM */
};
