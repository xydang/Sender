#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"

#include <stdio.h>
#include "uart.h"
#include "delay.h"
#include "imu.h"
#include "pwm.h"
#include "pwm_in.h"
#include "bmp180.h"

#define BYTE0(dwTemp)       (*(char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

#define UART_BUF_LENGTH 64
unsigned char uart_buf[UART_BUF_LENGTH];

unsigned char uart_busy_flag;

void uart_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);   
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_DeInit(DMA1_Channel4);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart_buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 100;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel4,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);

	//DMA_Cmd(DMA1_Channel4, ENABLE);

	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  
	USART_InitStructure.USART_Parity = USART_Parity_No;  
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    
	USART_InitStructure.USART_BaudRate = 115200; 
	USART_Init(USART1,&USART_InitStructure);  
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);  

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
	USART_Cmd(USART1, ENABLE);   

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
}

void DMA1_Channel4_IRQHandler(void)
{
	DMA_ClearFlag(DMA1_FLAG_TC4);
	DMA_Cmd(DMA1_Channel4,DISABLE);
	uart_busy_flag=0;
}

int fputc(int ch,FILE*F)
{
	USART_SendData(USART1,(unsigned char)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	return  (ch);
}

void uart_dma_send(unsigned char length)
{
	DMA_SetCurrDataCounter(DMA1_Channel4,length);
	DMA_Cmd(DMA1_Channel4,ENABLE);
}

void report_imu(void)
{
	int16_t temp;
	int32_t temp32;
	uint8_t sum=0,i=0,k=0;
	static int16_t alt,tempr,press;
	BMP180_Def  bmp;

	if(bmp180_read(&bmp)==0){
		bmp180_filter(&bmp);
		alt=(int16_t)bmp.altitude;
		tempr=(int16_t)bmp.temp;
		press=(int16_t)(bmp.pressure/10);
		imu.alt=alt;
		imu.press=press;
		imu.temp=tempr;
	}
	
	uart_buf[i++]=0xAA;
	uart_buf[i++]=0xAA;
	uart_buf[i++]=0x01;
	uart_buf[i++]=0x00;
	//roll
	temp= (int16_t)(euler.roll*100);
	uart_buf[i++]=BYTE1(temp);
	uart_buf[i++]=BYTE0(temp);
	//pitch
	temp= (int16_t)(euler.pitch*100);
	uart_buf[i++]=BYTE1(temp);
	uart_buf[i++]=BYTE0(temp);
	//yaw
	temp= (int16_t)(euler.yaw*100);
//	temp= (int16_t)(imu.heading*100);
	uart_buf[i++]=BYTE1(temp);
	uart_buf[i++]=BYTE0(temp);
	//Sonar
	temp= 0;
	uart_buf[i++]=BYTE1(temp);
	uart_buf[i++]=BYTE0(temp);
	//Pressure
	temp32= (int32_t)press*10;
	uart_buf[i++]=BYTE3(temp32);
	uart_buf[i++]=BYTE2(temp32);
	uart_buf[i++]=BYTE1(temp32);
	uart_buf[i++]=BYTE0(temp32);
	//ARMED
	if(rc_in.armed==0)
		uart_buf[i++]=0xA0;
	else
		uart_buf[i++]=0xA1;
	
	uart_buf[3]=i-4;
	//CheckSum
	for(k=0;k<i;k++)
		sum += uart_buf[k];
	uart_buf[i++]=sum;
	uart_dma_send(i);

}

