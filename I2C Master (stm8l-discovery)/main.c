/**
  ******************************************************************************
  * @file    I2C/I2C_TwoBoards/I2C_DataExchange/Master/main.c
  * @author  MCD Application Team
  * @version V1.5.1
  * @date    28-June-2013
  * @brief   Main program body
  ******************************************************************************
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "stm8l15x_it.h"
#include "main.h"
#include <string.h>
#include "discover_board.h"
#include "interboards_comm.h"
#include "crc8.h"

EXT_BOARD_PACKET_T buf;

void Delay(__IO uint32_t nCount)
{
  for (; nCount != 0; nCount--);
}

void blue_flash(uint8_t cnt){
  while(cnt--){
    GPIO_SetBits(LED_BLUE_PORT, LED_BLUE_PIN);
    Delay(0xFFFF/8);
    GPIO_ResetBits(LED_BLUE_PORT, LED_BLUE_PIN);
    Delay(0xFFFF/4);
  }
}

void green_flash(uint8_t cnt){
  while(cnt--){
    GPIO_SetBits(LED_GREEN_PORT, LED_GREEN_PIN);
    Delay(0xFFFF/8);
    GPIO_ResetBits(LED_GREEN_PORT, LED_GREEN_PIN);
    Delay(0xFFFF/4);
  }
}

void main(){
  /* I2C  clock Enable*/
  CLK_PeripheralClockConfig(CLK_Peripheral_I2C1, ENABLE);

#ifdef FAST_I2C_MODE
  /* system_clock / 1 */
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
#else
  /* system_clock / 2 */
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_2);
#endif

  Delay(0xFFFF);
  GPIO_Init(LED_GREEN_PORT, LED_GREEN_PIN, GPIO_Mode_Out_PP_Low_Slow);
  GPIO_Init(LED_BLUE_PORT, LED_BLUE_PIN, GPIO_Mode_Out_PP_Low_Slow);
  
  GPIO_Init(GPIOD, GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Slow); // GND pin for I2C communicaion
  
  /* Remap USART1 pins to PA2, PA3) */
  SYSCFG_REMAPDeInit();
  SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA, ENABLE);
      
  /* Initialize I2C peripheral */
  I2C_Init(I2C1, EXT_PLATE_I2C_SPEED, 0xA0,
           I2C_Mode_I2C, I2C_DutyCycle_2,
           I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);

  /* Enable Buffer and Event Interrupt*/
  I2C_ITConfig(I2C1, (I2C_IT_TypeDef)(I2C_IT_EVT | I2C_IT_BUF) , ENABLE);

  enableInterrupts();

  buf.CmdCode = CMD_SETUP_CODE;
  memset(&(buf.CmdSetup.PinConfig[0]), INPUT_PU, 21);
  memset(&(buf.CmdSetup.PowerOutConfig[0]), LOGICAL, 4);  
  buf.CmdSetup.CRC8 = crc8_comp((uint8_t*)(&buf.CmdSetup), 0xFF, sizeof(buf.CmdSetup) - 1);
  GPIO_SetBits(LED_BLUE_PORT, LED_BLUE_PIN);
  GPIO_ResetBits(LED_BLUE_PORT, LED_BLUE_PIN);
  I2C_transmit_buffer((uint8_t*)&(buf.CmdSetup),sizeof(buf.CmdSetup));

  
  memset(&buf, 0, sizeof(buf));
  buf.CmdCode = CMD_PUSH_CODE;
  buf.CmdPush.PowerOutState[1].DigitalOut = TRUE;
  buf.CmdPush.PowerOutState[2].DigitalOut = TRUE;
  buf.CmdPush.PowerOutState[3].DigitalOut = TRUE;  
  buf.CmdPush.CRC8 = crc8_comp((uint8_t*)(&buf.CmdPush), 0xFF, sizeof(buf.CmdPush) - 1);
  
  GPIO_SetBits(LED_BLUE_PORT, LED_BLUE_PIN);
  GPIO_ResetBits(LED_BLUE_PORT, LED_BLUE_PIN);
  I2C_transmit_buffer((uint8_t*)&(buf.CmdPush),sizeof(buf.CmdPush));
  
  GPIO_SetBits(LED_GREEN_PORT, LED_GREEN_PIN);
  while(1);
  buf.CmdCode = CMD_POLL_CODE;
  //TransmitBuf = (uint8_t*)(buf.CmdCode);  
  //BytesToTransmit = sizeof(buf.CmdCode);  
  /* Send START condition */
  I2C_GenerateSTART(I2C1, ENABLE);
  //while (BytesToTransmit);
  while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
  
  
  //TransmitBuf = (uint8_t*)&(buf.CmdPoll);
  //BytesToTransmit = sizeof(buf.CmdPoll) - 1;
  I2C_GenerateSTART(I2C1, ENABLE);
  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

  /* Send slave Address for write */
  I2C_Send7bitAddress(I2C1, (EXT_PLATE_SLAVE_ADDR<<1), I2C_Direction_Receiver);

  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

#if 0
  /* While there is data to be read */
  while (BytesToTransmit)
  {
#ifdef SAFE_PROCEDURE
    if (BytesToTransmit != 3) /* Receive bytes from first byte until byte N-3 */
    {
      while ((I2C_GetLastEvent(I2C1) & 0x04) != 0x04); /* Poll on BTF */

      /* Read a byte from the Slave */
      *TransmitBuf++ = I2C_ReceiveData(I2C1);

      /* Decrement the read bytes counter */
      BytesToTransmit--;
    }

    if (BytesToTransmit == 3)  /* it remains to read three data: data N-2, data N-1, Data N */
    {
      /* Data N-2 in DR and data N -1 in shift register */
      while ((I2C_GetLastEvent(I2C1) & 0x04) != 0x04); /* Poll on BTF */

      /* Clear ACK */
      I2C_AcknowledgeConfig(I2C1, DISABLE);

      /* Disable general interrupts */
      disableInterrupts();

      /* Read Data N-2 */
      *TransmitBuf++ = I2C_ReceiveData(I2C1);

      /* Program the STOP */
      I2C_GenerateSTOP(I2C1, ENABLE);

      /* Read DataN-1 */
      *TransmitBuf++ = I2C_ReceiveData(I2C1);

      /* Enable General interrupts */
      enableInterrupts();

      while ((I2C_GetLastEvent(I2C1) & 0x40) != 0x40); /* Poll on RxNE */

      /* Read DataN */
      *TransmitBuf++ = I2C_ReceiveData(I2C1);

      /* Reset the number of bytes to be read by master */
      BytesToTransmit = 0;

    }
#else
    if (BytesToTransmit == 1)
    {
      /* Disable Acknowledgement */
      I2C_AcknowledgeConfig(I2C1, DISABLE);

      /* Send STOP Condition */
      I2C_GenerateSTOP(I2C1, ENABLE);

      /* Poll on RxNE Flag */
      while ((I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET));
      /* Read a byte from the Slave */
      *TransmitBuf++ = I2C_ReceiveData(I2C1);

      /* Decrement the read bytes counter */
      BytesToTransmit--;
    }

    /* Test on EV7 and clear it */
    if (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) )
    {
      /* Read a byte */
      *TransmitBuf++ = I2C_ReceiveData(I2C1);

      /* Decrement the read bytes counter */
      BytesToTransmit--;
    }
#endif /* SAFE_PROCEDURE */
  }
#endif
  while(1)
  {
    green_flash(1);
  }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/