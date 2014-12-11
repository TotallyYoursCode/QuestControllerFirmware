/**
  ******************************************************************************
  * @file    ExtPlate/main.c 
  * @author  Kiselev.E
  * @version V1.0.0
  * @date    06-December-2014
  * @brief   Main program body
  ******************************************************************************
  */ 

#include "stm8s.h"
#include "stm8s_it.h"
#include "interboards_comm.h"
#include "board.h"
#include "stdio.h"
#define PUTCHAR_PROTOTYPE int putchar (int c)


EXT_BOARD_PACKET_T Buf;

EXT_PIN_MODE_T              PinConfig[__EXT_PIN_COUNT];
EXT_PIN_STATE_T             PinState[__EXT_PIN_COUNT];
EXT_POWER_OUT_MODE_T        PowerOutConfig[__EXT_POWER_OUT_COUNT];
EXT_POWER_OUT_STATE_T       PowerOutState[__EXT_POWER_OUT_COUNT];



void main(void){
    uint8_t i;
    /* Set HSI (High Speed Internal) oscillator prescaler to 1 */
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    
    UART2_DeInit();
    UART2_Init((uint32_t)256000, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, 
               UART2_PARITY_NO, UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TX_ENABLE);
    
    printf("External Pins Plate Terminal module initiated\n\r");
    
    for(i = 0; i<__EXT_PIN_COUNT; i++){
      GPIO_Init(ExtPinGPIOPort[i], ExtPinNum[i], GPIO_MODE_IN_FL_NO_IT);
    }
    
    for(i = 0; i<__EXT_POWER_OUT_COUNT; i++){
      GPIO_Init(ExtPowerOutGPIOPort[i], ExtPowerOutNum[i], GPIO_MODE_OUT_PP_LOW_FAST);
    }    
    printf("pins initialized\n\r");
    
    GPIOE->CR1 |= 0x06;
    GPIOE->DDR &= ~0x06;
    GPIOE->CR2 &= ~0x06;
    
    I2C_DeInit();
    I2C_Init(EXT_PLATE_I2C_SPEED, (EXT_PLATE_SLAVE_ADDR<<1), I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);           
    I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_ERR | I2C_IT_EVT | I2C_IT_BUF), ENABLE);
    printf("i2c initialized\n\r");
        
    enableInterrupts();
    while (1){
      if(I2C_get_data((uint8_t*)&Buf)){
        if(Buf.CmdCode == CMD_SETUP_CODE){
          printf("SETUP code received\n\r");
          printf("ExtPins:\n\r");
          for(i = 0; i<__EXT_PIN_COUNT; i++){
            if(Buf.CmdSetup.PinConfig[i] != NOT_USED){
              GPIO_Init(ExtPinGPIOPort[i], ExtPinNum[i], ExtPinMode[Buf.CmdSetup.PinConfig[i]]);
              printf("   ExtPin: %d init as %d\n\r", i, Buf.CmdSetup.PinConfig[i]);
            }
          }
          printf("ExtPowerOuts:\n\r");
          for(i = 0; i<__EXT_POWER_OUT_COUNT; i++){
            GPIO_Init(ExtPowerOutGPIOPort[i], ExtPowerOutNum[i], ExtPowerOutMode[Buf.CmdSetup.PowerOutConfig[i]]);
            printf("   ExtPowerOut: %d init as %d\n\r", i, Buf.CmdSetup.PowerOutConfig[i]);
          }
        }
        if(Buf.CmdCode == CMD_PUSH_CODE){
          printf("PUSH code received\n\r");
          printf("ExtPins:\n\r");
          for(i = 0; i<__EXT_PIN_COUNT; i++){
            if(Buf.CmdPush.PinState[i].DigitalOut == 1){
              GPIO_WriteHigh(ExtPinGPIOPort[i], ExtPinNum[i]);
              printf("   ExtPin: %d write 1\n\r", i);
            } else {
              GPIO_WriteLow(ExtPinGPIOPort[i], ExtPinNum[i]);
              printf("   ExtPin: %d write 0\n\r", i);
            }
          }
          printf("ExtPowerOuts:\n\r");
          for(i = 0; i<__EXT_POWER_OUT_COUNT; i++){
            if(Buf.CmdPush.PowerOutState[i].DigitalOut == 1){
              GPIO_WriteHigh(ExtPowerOutGPIOPort[i], ExtPowerOutNum[i]);
              printf("   ExtPowerOut: %d write 1\n\r", i);
            } else {
              GPIO_WriteLow(ExtPowerOutGPIOPort[i], ExtPowerOutNum[i]);
              printf("   ExtPowerOut: %d write 0\n\r", i);
            }
          }
        }
      }
    }
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

