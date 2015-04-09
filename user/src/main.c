#include "stm32f10x.h"
#include <stdint.h>
#include <stdio.h>
#include "delay.h"
#include "uart.h"
#include "timer.h"
#include "i2c.h"
#include "mpu6050.h"
#include "hmc5883.h"
#include "bmp180.h"
#include "pwm.h"
#include "pwm_in.h"
#include "imu.h"
#include "pid.h"


void gpio_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void board_init()
{
	uint8_t i;
	delay_ms(100);
	SystemInit();
	systick_init();
	gpio_init();
	uart_init();
	pwm_init();
	pwm_in_init();
	i2c_init();
	printf(" Board Init OK... \r\n");
	mpu6050_init();
	hmc5883_init();
	bmp180_init();
	printf(" Sensor Init OK... \r\n");
	for(i=0;i<20;i++){
		imu_read(&imu);
		quat_init(&imu,&quat,&euler);
	}
	pid_init();
	printf(" Quat Init OK... \r\n");
	delay_ms(8000);
	delay_ms(8000);
}


void blink()
{
	if(cnt_control>=500){
		cnt_control=0;
		if(led_fleg){
			led_fleg=0;
			GPIO_WriteBit(GPIOB, GPIO_Pin_3,Bit_RESET);
		}else{
			led_fleg=1;
			GPIO_WriteBit(GPIOB, GPIO_Pin_3,Bit_SET);
		}
	}
}

void imu_control()
{
	if(cnt_imu>=4){
		cnt_imu=0;
		imu_read(&imu);
		imu_update(&imu,&quat,&euler);
		pid_control();
//		bmp180_read(&bmp);
//		bmp180_filter(&bmp);
	}
}

int main()
{
	board_init();
	while(1){
		blink();
		imu_control();
		if(cnt_report>=50){
			cnt_report=0;
			report_imu();
		}
	}
}

