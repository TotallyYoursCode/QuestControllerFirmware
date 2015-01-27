#include "uart_debug.h"
#include "stm8s.h"


static volatile uint8_t buf[DEBUG_BUF_LEN], head, tail, cnt;

void uartDebugInit(void)
{
	ITC_SetSoftwarePriority(ITC_IRQ_UART2_TX, ITC_PRIORITYLEVEL_1);	// the lowest priority
	UART2_DeInit();
  	UART2_Init((uint32_t)256000, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, UART2_PARITY_NO,
             	UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TX_ENABLE);	
  
  	/* Enable the UART Transmit interrupt */
  	UART2->CR2 |= UART2_CR2_TIEN;

  	/* Enable UART */
    UART2->CR1 &= (uint8_t)(~UART2_CR1_UARTD);
	head = tail = cnt = 0;
}
	

int putchar(int c)
{
	UART2->CR2 &= (~UART2_CR2_TIEN);
	if (cnt < DEBUG_BUF_LEN) {
		cnt++;
		buf[tail] = c;
		if (++tail >= DEBUG_BUF_LEN) {
			tail = 0;
		}
	}
	UART2->CR2 |= UART2_CR2_TIEN;
	return c;
}

 INTERRUPT_HANDLER(UART2_TX_IRQHandler, 20)
 {
	 if (cnt > 0) {
		 cnt--;
		 UART2->DR = buf[head];
		 if (++head >= DEBUG_BUF_LEN) {
			 head = 0;
		 }
	 } else {
		 UART2->CR2 &= (~UART2_CR2_TIEN);
	 }
 }