/**
  ******************************************************************************
  * @file    ExtPlate/i2c_protocol.h 
  * @author  Kiselev.E
  * @version V1.0.0
  * @date    06-December-2014
  * @brief   i2c protocol header
  ******************************************************************************
  */ 

#ifndef _INTERBOARDS_COMM_
#define _INTERBOARDS_COMM_

#ifdef AVR_MCU
#include <stdint.h>
#elif defined(STM8S105)
#include "stm8s.h"
#elif defined(STM8L15X_MD)
#include "stm8l15x.h"
#endif

#define EXT_PLATE_SLAVE_ADDR    (0x10)     // 7-bit value, must be shifted left in STM
#define EXT_PLATE_I2C_SPEED     (380000)

#define ANALOG_OUT_COUNT          (3)
#define ANALOG_IN_COUNT           (4)

/**      All possible functions for External Pins
                DI      DO      AI      AO
ExtPin_0        *       -       -       -
ExtPin_1        *       -       *       -
ExtPin_2        *       -       *       -
ExtPin_3        *       -       *       -
ExtPin_4        *       *       *       -
ExtPin_5        *       *       *       -
ExtPin_6        *       *       *       -
ExtPin_7        *       *       *       -
ExtPin_8        *       *       *       -
ExtPin_9        *       *       *       -
ExtPin_10       *       *       *       -
ExtPin_11       *(s)    *(s)    -       *
ExtPin_12       *(s)    *(s)    -       *
ExtPin_13       *(s)    *(s)    -       *
ExtPin_14       *       *       -       -
ExtPin_15       *       *       -       -
ExtPin_16       *       *       -       -
ExtPin_17       *       *       -       -
ExtPin_18       *       *       -       -
ExtPin_19       *       *       -       -
ExtPin_20       *       *       -       -

 (s) - slow, because of 0,1uF capasitor on pin.
*/
/* Extrenal pins names */
typedef enum{
    EXT_PIN_0 = 0,
    EXT_PIN_1,
    EXT_PIN_2,
    EXT_PIN_3,
    EXT_PIN_4,
    EXT_PIN_5,
    EXT_PIN_6,
    EXT_PIN_7,
    EXT_PIN_8,
    EXT_PIN_9,
    EXT_PIN_10,
    EXT_PIN_11,
    EXT_PIN_12,
    EXT_PIN_13,
    EXT_PIN_14,
    EXT_PIN_15,
    EXT_PIN_16,
    EXT_PIN_17,
    EXT_PIN_18,
    EXT_PIN_19,
    EXT_PIN_20,
    __EXT_PIN_COUNT
} EXT_PIN_NAME_T;
#define PIN_CONF_ARR_SIZE       ((__EXT_PIN_COUNT+1)/2)
#define PIN_STATE_ARR_SIZE      ((__EXT_PIN_COUNT+4)/8)
#define CAN_BE_ANALOG_OUT(a)    ((a>=EXT_PIN_11)&&(a<=EXT_PIN_13))
#define CAN_BE_ANALOG_IN(a)     ((a>=EXT_PIN_1)&&(a<=EXT_PIN_10))
#define IS_PROTECTED_IN(a)      ((a>=EXT_PIN_0)&&(a<=EXT_PIN_3))
#define CAN_BE_DIGITAL_OUT(a)   (!(IS_PROTECTED_IN(a)))


/* All External power outputs can work in two possible modes: LOGICAL or PWM */
/* External power pin names */
typedef enum{
  EXT_POWER_OUT_0 = 0,
  EXT_POWER_OUT_1,
  EXT_POWER_OUT_2,
  EXT_POWER_OUT_3,
  __EXT_POWER_OUT_COUNT
} EXT_POWER_OUT_NAME_T;

/* Extrenal pins modes */
typedef enum{
    NOT_USED = 0,
    OUTPUT_PP,
    OUTPUT_OD,
    INPUT_PU,
    INPUT_Z,
    ANALOG_IN,
    ANALOG_OUT,
    __EXT_PIN_MODE_COUNT
} EXT_PIN_MODE_T;
#define DIGITAL_OUTPUT_DEFAULT_MODE     (OUTPUT_PP)
#define IS_DIGITAL_OUTPUT(a)            ((a==OUTPUT_PP)||(a==OUTPUT_OD))
#define IS_DIGITAL_INPUT(a)             ((a==INPUT_PU)||(a==INPUT_PD)||(a==INPUT_Z))

/* Extrenal power pins modes */
typedef enum{
  LOGICAL = 0,
  PWM,
  __EXT_POWER_OUT_MODE_COUNT
} EXT_POWER_OUT_MODE_T;

/* Commands list */
typedef enum{
  CMD_SETUP_CODE = 0,
  CMD_PUSH_CODE,
  CMD_POLL_CODE
} EXT_BOARD_CMD_CODE_T;

typedef union{
  uint8_t AnalogIn;
  uint8_t AnalogOut;
  bool    DigitalIn;
  bool    DigitalOut;  
}EXT_PIN_STATE_T;

typedef union{
  uint8_t PWM;
  bool DigitalOut;
}EXT_POWER_OUT_STATE_T;

/* Setup packet structure */
typedef struct{
    EXT_BOARD_CMD_CODE_T CmdCode;
    /* uint8_t     PinConfig[PIN_CONF_ARR_SIZE]; */
    EXT_PIN_MODE_T       PinConfig[__EXT_PIN_COUNT];
    EXT_POWER_OUT_MODE_T PowerOutConfig[__EXT_POWER_OUT_COUNT];
    uint8_t     CRC8;
} EXT_BOARD_CMD_SETUP_T;

/* Push packet structure */
typedef struct{
    EXT_BOARD_CMD_CODE_T CmdCode;
    /* uint8_t     AnalogOut[ANALOG_OUT_COUNT];
    uint8_t     DigitalOutState[PIN_STATE_ARR_SIZE];
    uint8_t     PowerOuts; */
    EXT_PIN_STATE_T       PinState[__EXT_PIN_COUNT];
    EXT_POWER_OUT_STATE_T PowerOutState[__EXT_POWER_OUT_COUNT];
    uint8_t     CRC8;
} EXT_BOARD_CMD_PUSH_T;

/* Poll packet structure */
typedef struct{
    EXT_BOARD_CMD_CODE_T CmdCode;
    /* uint8_t     AnalogInput[ANALOG_IN_COUNT];
    uint8_t     DigitalInputState[PIN_STATE_ARR_SIZE]; */
    EXT_PIN_STATE_T       PinState[__EXT_PIN_COUNT];
    uint8_t     CRC8;
} EXT_BOARD_CMD_POLL_T;

/* Combined all packets in one union */
typedef union{
    EXT_BOARD_CMD_CODE_T   CmdCode;
    EXT_BOARD_CMD_SETUP_T  CmdSetup;
    EXT_BOARD_CMD_PUSH_T   CmdPush;
    EXT_BOARD_CMD_POLL_T   CmdPoll;
} EXT_BOARD_PACKET_T;


#endif