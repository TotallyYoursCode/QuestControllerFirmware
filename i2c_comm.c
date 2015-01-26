/**
  ******************************************************************************
  * @file    ExtPlate/i2c_comm.c
  * @author  Kiselev.E
  * @version V1.0.0
  * @date    06-December-2014
  * @brief   i2c communication routines file
  ******************************************************************************
  */

#include "i2c_comm.h"
#include "string.h"
#include "stdio.h"
#include "crc8.h"

#define check_bit(val,bit)      ((val) &   (1<<(bit)))
#define clear_bit(val,bit)      ((val) &= ~(1<<(bit)))
#define set_bit(val,bit)        ((val) |=  (1<<(bit)))

static EXT_BOARD_PACKET_T RecBuf[2], TrBuf[_TRANSMIT_PACKET_NUM];
static uint8_t  RecBufLen[2];
static uint8_t *pRecBuf, *pTrBuf;
static uint8_t  BytesToTransmit,
       BytesReceived,
       RecBufNum,
       RecBufFullFlag;


void I2C_init(void)
{
    /* GPIO Port init */
    GPIOE->CR1 |=   (1 << 2) | (1 << 1);
    GPIOE->DDR &= ~((1 << 2) | (1 << 1));
    GPIOE->CR2 &= ~((1 << 2) | (1 << 1));
    I2C_DeInit();
    I2C_Init(EXT_PLATE_I2C_SPEED, (EXT_PLATE_SLAVE_ADDR << 1), I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);
    I2C_ITConfig((I2C_IT_TypeDef)(I2C_IT_ERR | I2C_IT_EVT | I2C_IT_BUF), ENABLE);
    pRecBuf = (uint8_t *)&RecBuf[0];
    pTrBuf  = (uint8_t *)&TrBuf[0];
    BytesToTransmit = BytesReceived = RecBufNum = 0;
    RecBufFullFlag = 0;
}


uint8_t I2C_get_data(EXT_BOARD_PACKET_T *buf)
{
    uint8_t len;
    if (check_bit(RecBufFullFlag, 0)) {
        memcpy(buf, &RecBuf[0], sizeof(EXT_BOARD_PACKET_T));
        len = RecBufLen[0];
        RecBufLen[0] = 0;
        clear_bit(RecBufFullFlag, 0);
        return len;
    }
    if (check_bit(RecBufFullFlag, 1)) {
        memcpy(buf, &RecBuf[1], sizeof(EXT_BOARD_PACKET_T));
        len = RecBufLen[1];
        RecBufLen[1] = 0;
        clear_bit(RecBufFullFlag, 1);
        return len;
    }
    return 0;
}

uint8_t I2C_set_data(TRANSMIT_PACKET_TYPE cmd, EXT_BOARD_PACKET_T *buf)
{
    memcpy(&TrBuf[cmd], buf, sizeof(EXT_BOARD_PACKET_T));
    return 1;
}


void check_flags(uint8_t sr1, uint8_t sr2, uint8_t sr3)
{
    printf("SR1: ");
    if (sr1 & I2C_SR1_TXE) printf("TXE ");
    if (sr1 & I2C_SR1_RXNE) printf("RXNE ");
    if (sr1 & I2C_SR1_STOPF) printf("STOPF ");
    if (sr1 & I2C_SR1_ADD10) printf("ADD10 ");
    if (sr1 & I2C_SR1_BTF) printf("BTF ");
    if (sr1 & I2C_SR1_ADDR) printf("ADDR ");
    if (sr1 & I2C_SR1_SB) printf("SB ");
    printf("\r\n");

    printf("SR2: ");
    if (sr2 & I2C_SR2_WUFH) printf("WUFH ");
    if (sr2 & I2C_SR2_OVR) printf("OVR ");
    if (sr2 & I2C_SR2_AF) printf("AF ");
    if (sr2 & I2C_SR2_ARLO) printf("ARLO ");
    if (sr2 & I2C_SR2_BERR) printf("BERR ");
    printf("\r\n");

    printf("SR3: ");
    if (sr3 & I2C_SR3_GENCALL) printf("GENCALL ");
    if (sr3 & I2C_SR3_TRA) printf("TRA ");
    if (sr3 & I2C_SR3_BUSY) printf("BUSY ");
    if (sr3 & I2C_SR3_MSL) printf("MSL ");
    printf("\r\n");
}


void pulse(uint8_t cnt)
{
    GPIO_Init(GPIOB, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST);
    while (cnt--) {
        GPIO_WriteHigh(GPIOB, GPIO_PIN_2);
        GPIO_WriteLow(GPIOB, GPIO_PIN_2);
    }
}


/**
  * @brief I2C Interrupt routine.
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(I2C_IRQHandler, 19)
{
    //printf("   i2c ISR:\r\n");
    volatile uint8_t sr1, sr2, sr3;

    // save the I2C registers configuration
    sr1 = I2C->SR1;
    sr2 = I2C->SR2;
    sr3 = I2C->SR3;
    //check_flags(sr1, sr2, sr3);

    // ## Common flags
    /* Communication error? */
    if (sr2 & (I2C_SR2_WUFH | I2C_SR2_OVR | I2C_SR2_ARLO | I2C_SR2_BERR)) {
        I2C->CR2 |= I2C_CR2_STOP;   // stop communication - release the lines
        I2C->SR2 = 0;               // clear all error flags
        printf("Comm Error");
        //return;
    }

    /* Data register not empty */
    if (sr1 & I2C_SR1_RXNE) {
		if (BytesReceived < sizeof(EXT_BOARD_PACKET_T)) {
			BytesReceived++;
			*pRecBuf++ = I2C->DR;
			if (sr1 & I2C_SR1_BTF) {
				if (BytesReceived < sizeof(EXT_BOARD_PACKET_T)) {
					BytesReceived++;
					*pRecBuf++ = I2C->DR;
				} else {
					I2C->DR;
				}
			}
		} else {
			I2C->DR;
		}
        //return;
    }
    /* NAK from master ( == end of slave transmit comm) */
    if (sr2 & I2C_SR2_AF) {
        I2C->SR2 &= ~I2C_SR2_AF;      // clear AF
        BytesToTransmit = 0;
        //return;
    }

    /* Stop bit from Master  (End of slave receive comm) */
    if (sr1 & I2C_SR1_STOPF) {
        I2C->CR2 |= I2C_CR2_ACK;
        set_bit(RecBufFullFlag, RecBufNum);
        RecBufLen[RecBufNum] = BytesReceived;
        //return;
    }

    /* Slave address matched (Start Comm) */
    if (sr1 & I2C_SR1_ADDR) {
        //printf("Address matched\r\n");
        if (sr3 & I2C_SR3_TRA) {
            //printf("Transmit started\r\n");
            /* Transmit comm started */
            switch (RecBuf[RecBufNum].CmdCode) {
            case CMD_POLL_CODE:
                //printf("POLL CODE\r\n");
                pTrBuf = (uint8_t *)&TrBuf[POLL];
                BytesToTransmit = sizeof(((EXT_BOARD_PACKET_T *)pTrBuf)->CmdPoll);
                //printf("Bytes = %d\r\n", BytesToTransmit);
                break;
            default:
                break;
            }
        } else {
            //printf("Receive started\r\n");
            /* Receive comm started */
            if (!(RecBufFullFlag & 0x01)) {
                pRecBuf = (uint8_t *)&RecBuf[0];
                RecBufNum = 0;
            } else if (!(RecBufFullFlag & 0x02)) {
                pRecBuf = (uint8_t *)&RecBuf[1];
                RecBufNum = 1;
            } else {
                I2C->CR2 |= I2C_CR2_STOP;   // stop communication - release the lines
                I2C->SR2 = 0;               // clear all error flags
            }
            BytesReceived = 0;
        }
        //return;
    }

    /* Data register empty */
    if (sr1 & I2C_SR1_TXE) {
        if (BytesToTransmit) {
            I2C->DR = *pTrBuf++;
            BytesToTransmit--;
            if (sr1 & I2C_SR1_BTF) {
                if (BytesToTransmit) {
                    I2C->DR = *pTrBuf++;
                    BytesToTransmit--;
                } else {
					I2C->DR = 0x00;
				}
            }
        } else {
            I2C->DR = 0x00;
        }
        //return;
    }
}