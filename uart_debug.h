#ifndef _UART_DEBUG_H_
#define _UART_DEBUG_H_

#define DEBUG_BUF_LEN	(128)
#define PUTCHAR_PROTOTYPE int putchar (int c)

int putchar(int c);
void uartDebugInit(void);

#endif