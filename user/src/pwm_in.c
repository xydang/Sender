#include <stdint.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "pwm_in.h"
/*
    RC1  TIM2_CH1 PA0
    RC2  TIM2_CH2 PA1
    RC3  TIM3_CH1 PA6
    RC4  TIM3_CH2 PA7
    RC5  TIM3_CH3 PB0
    RC6  TIM3_CH4 PB1
*/
static struct TIM_Channel 
{
	TIM_TypeDef *tim;
	uint16_t channel;
	uint16_t cc;
} Channels[] = {
	{ TIM2, TIM_Channel_1, TIM_IT_CC1 },
	{ TIM2, TIM_Channel_2, TIM_IT_CC2 },
	{ TIM3, TIM_Channel_1, TIM_IT_CC1 },
	{ TIM3, TIM_Channel_2, TIM_IT_CC2 },
	{ TIM3, TIM_Channel_3, TIM_IT_CC3 },
	{ TIM3, TIM_Channel_4, TIM_IT_CC4 }
};

static TIM_ICInitTypeDef  TIM_ICInitStructure; 
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

PWM_Input Inputs[6];
RC_Input rc_in;

void pwm_in_init()
{
	uint8_t i;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_DeInit(TIM2);
	TIM_DeInit(TIM3);

	TIM_TimeBaseStructure.TIM_Prescaler = (72 - 1);
	TIM_TimeBaseStructure.TIM_Period = 0xffff;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);	
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0xF;
	for(i=0;i<6;i++){
		TIM_ICInitStructure.TIM_Channel = Channels[i].channel;
		TIM_ICInit(Channels[i].tim, &TIM_ICInitStructure);
	}
	TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_CC2 , ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4 , ENABLE);

	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	
//	Inputs[0].capture  =  roll_init;
//	Inputs[1].capture  =  pitch_init;
//	Inputs[2].capture  =  thr_init;
//	Inputs[3].capture  =  yaw_init;	
	
}

void TIM2_IRQHandler(void)
{
	uint16_t val =0,i=0;
	for(i=0;i<2;i++){
		if (Channels[i].tim == TIM2 && (TIM_GetITStatus(TIM2, Channels[i].cc) == SET)){
			TIM_ClearITPendingBit(TIM2, Channels[i].cc);
			switch (Channels[i].channel)
			{
				case TIM_Channel_1:
					val = TIM_GetCapture1(TIM2);
					break;
				case TIM_Channel_2:
					val = TIM_GetCapture2(TIM2);
					break;
			}
			if (Inputs[i].state == 0)
				Inputs[i].rise = val;
			else
				Inputs[i].fall = val;
			if (Inputs[i].state == 0) {
				Inputs[i].state = 1;
				TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
				TIM_ICInitStructure.TIM_Channel = Channels[i].channel;
				TIM_ICInit(TIM2, &TIM_ICInitStructure);
			}else{
				if(Inputs[i].fall > Inputs[i].rise ){
					val = (Inputs[i].fall - Inputs[i].rise);
					if(val < 2100 && val>900)
						Inputs[i].capture = val;
				}else{
					val = ((0xffff - Inputs[i].rise) + Inputs[i].fall);
					if(val < 2100 && val>900)
					Inputs[i].capture = val;
				}
				Inputs[i].state = 0;
				TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
				TIM_ICInitStructure.TIM_Channel = Channels[i].channel;
				TIM_ICInit(TIM2, &TIM_ICInitStructure);
			}
		}
	}
}

void TIM3_IRQHandler(void)
{
	uint16_t val =0,i=0;
	for(i=0;i<4;i++){
		if (Channels[i+2].tim == TIM3 && (TIM_GetITStatus(TIM3, Channels[i+2].cc) == SET)){
			TIM_ClearITPendingBit(TIM3, Channels[i+2].cc);
			switch (Channels[i+2].channel)
			{
				case TIM_Channel_1:
					val = TIM_GetCapture1(TIM3);
					break;
				case TIM_Channel_2:
					val = TIM_GetCapture2(TIM3);
					break;
				case TIM_Channel_3:
					val = TIM_GetCapture3(TIM3);
					break;
				case TIM_Channel_4:
					val = TIM_GetCapture4(TIM3);
					break;
			}
			if (Inputs[i+2].state == 0)
				Inputs[i+2].rise = val;
			else
				Inputs[i+2].fall = val;
			if (Inputs[i+2].state == 0) {
				Inputs[i+2].state = 1;
				TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
				TIM_ICInitStructure.TIM_Channel = Channels[i+2].channel;
				TIM_ICInit(TIM3, &TIM_ICInitStructure);
			}else{
				if(Inputs[i+2].fall > Inputs[i+2].rise ){
					val = (Inputs[i+2].fall - Inputs[i+2].rise);
					if(val < 2100 && val>900)
						Inputs[i+2].capture = val;
				}else{
					val = ((0xffff - Inputs[i+2].rise) + Inputs[i+2].fall);
					if(val < 2100 && val>900)
					Inputs[i+2].capture = val;
				}
				Inputs[i+2].state = 0;
				TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
				TIM_ICInitStructure.TIM_Channel = Channels[i+2].channel;
				TIM_ICInit(TIM3, &TIM_ICInitStructure);
			}
		}
	}
	rc_in.thr=Inputs[0].capture;
	rc_in.roll=Inputs[1].capture;
	rc_in.pitch=Inputs[2].capture;
	rc_in.yaw=Inputs[3].capture;
	rc_in.mode=Inputs[4].capture;
	rc_in.serve=Inputs[5].capture;
}


