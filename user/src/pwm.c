#include <stdint.h>
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "pwm.h"

uint8_t unlock_flag=0;

int16_t MOTO1_PWM=UNLOCK_THROTTLE;
int16_t MOTO2_PWM=UNLOCK_THROTTLE;
int16_t MOTO3_PWM=UNLOCK_THROTTLE;
int16_t MOTO4_PWM=UNLOCK_THROTTLE;
int16_t MOTO5_PWM=UNLOCK_THROTTLE;
int16_t MOTO6_PWM=UNLOCK_THROTTLE;

int16_t MOTO_PWM_MAX=MAX_THROTTLE;
int16_t MOTO_PWM_MIN=MIN_THROTTLE;

static void PWM_GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

static void PWM_Mode_Config()
{
//	uint16_t CCR1_Val=UNLOCK_THROTTLE;
//	uint16_t CCR2_Val=UNLOCK_THROTTLE;
	uint16_t CCR3_Val=UNLOCK_THROTTLE;
	uint16_t CCR4_Val=UNLOCK_THROTTLE;
	uint16_t CCR5_Val=UNLOCK_THROTTLE;
	uint16_t CCR6_Val=UNLOCK_THROTTLE;
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	TIM_TimeBaseStructure.TIM_Period=20000;
	TIM_TimeBaseStructure.TIM_Prescaler=72 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision=0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
	
	TIM_OCInitStructure.TIM_Pulse=CCR3_Val;
	TIM_OC1Init(TIM4,&TIM_OCInitStructure);//enable channel 3
	TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_Pulse=CCR4_Val;
	TIM_OC2Init(TIM4,&TIM_OCInitStructure);//enable channel 4
	TIM_OC2PreloadConfig(TIM4,TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_Pulse=CCR5_Val;
	TIM_OC3Init(TIM4,&TIM_OCInitStructure);//enable channel 5
	TIM_OC3PreloadConfig(TIM4,TIM_OCPreload_Enable);

	TIM_OCInitStructure.TIM_Pulse=CCR6_Val;
	TIM_OC4Init(TIM4,&TIM_OCInitStructure);//enable channel 6
	TIM_OC4PreloadConfig(TIM4,TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM4,ENABLE);//reloadarr
	TIM_Cmd(TIM4,ENABLE);//enable timer 4
	
	/**********************************/
/*
//	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_Pulse=CCR1_Val;
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);//enable channel 1
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);
	
	TIM_OCInitStructure.TIM_Pulse=CCR2_Val;
	TIM_OC4Init(TIM1,&TIM_OCInitStructure);//enable channel 2
	TIM_OC4PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_Cmd(TIM1,ENABLE);//enable timer 1
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
*/
}

void pwm_init(void)
{
	PWM_GPIO_Config();
	PWM_Mode_Config();
}

void pwm_set(int16_t motor1,int16_t motor2,int16_t motor3,int16_t motor4,int16_t motor5,int16_t motor6)
{
	if(MOTO1_PWM>=MOTO_PWM_MAX)	MOTO1_PWM = MOTO_PWM_MAX;
	if(MOTO2_PWM>=MOTO_PWM_MAX)	MOTO2_PWM = MOTO_PWM_MAX;
	if(MOTO3_PWM>=MOTO_PWM_MAX)	MOTO3_PWM = MOTO_PWM_MAX;
	if(MOTO4_PWM>=MOTO_PWM_MAX)	MOTO4_PWM = MOTO_PWM_MAX;

	if(MOTO1_PWM<=MOTO_PWM_MIN)	MOTO1_PWM = MOTO_PWM_MIN;
	if(MOTO2_PWM<=MOTO_PWM_MIN)	MOTO2_PWM = MOTO_PWM_MIN;
	if(MOTO3_PWM<=MOTO_PWM_MIN)	MOTO3_PWM = MOTO_PWM_MIN;
	if(MOTO4_PWM<=MOTO_PWM_MIN)	MOTO4_PWM = MOTO_PWM_MIN;
	
	TIM1->CCR1 = motor5;    //TIM1_CH1_MOTO1	PA8
	TIM1->CCR4 = motor6;	//TIM1_CH4_MOTO2	PA11
	TIM4->CCR1 = motor1;	//TIM4_CH1_MOTO3	PB6
	TIM4->CCR2 = motor2;	//TIM4_CH2_MOTO4	PB7	
	TIM4->CCR3 = motor3;	//TIM4_CH3_MOTO3	PB8
	TIM4->CCR4 = motor4;	//TIM4_CH4_MOTO4	PB9
}
