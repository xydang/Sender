#include "stm32f10x.h"
#include <stdint.h>
#include "delay.h"

uint16_t cnt_control=0,cnt_imu=0,cnt_report=0;

void systick_init()
{
	SysTick_Config(72000);
}

void delay_ms(uint16_t t)
{
	uint16_t i;
	for(;t>0;t--)
		for(i=1200;i>0;i--);
}

void delay_us(uint16_t t)
{
	uint16_t i;
	for(;t>0;t--)
		for(i=10;i>0;i--);
}

void SysTick_Handler(void)
{
	cnt_control++;
	cnt_imu++;
	cnt_report++;
}
