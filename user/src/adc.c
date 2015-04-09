#include <stdint.h>
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "adc.h"

#define ADC_CHN_NUM 			2
#define ADC1_DR_Address    ((uint32_t)0x4001244C)

volatile uint16_t ADC_Value[ADC_CHN_NUM];

static void ADC_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}

static void ADC_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr=ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)ADC_Value;
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize=ADC_CHN_NUM;
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1,ENABLE);
}

static void ADC_Mode_Config(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode=ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel=2;
	ADC_Init(ADC1,&ADC_InitStructure);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_10,1,ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_11,2,ADC_SampleTime_55Cycles5);
	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1,ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	
}

void ADC_Config(void)
{
	ADC_GPIO_Config();
	ADC_DMA_Config();
	ADC_Mode_Config();
}
