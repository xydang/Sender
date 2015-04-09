#ifndef __UART_H__
#define __UART_H__
//void USART_Config(void);
void uart_init(void);
int fputc(int ch,FILE*F);
void report_imu(void);
void report_motion(void);
void DMA1_Channel4_IRQHandler(void);
#endif

