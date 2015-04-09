#include <stdint.h>
#include <stdio.h>
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"
#include "i2c.h"
#include "delay.h"

#ifndef SOFT_I2C

void i2c_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;
	
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
	
	I2C_DeInit(I2C2);
	I2C_InitStructure.I2C_Mode=I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle=I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1=0x00;
	I2C_InitStructure.I2C_Ack=I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed=I2C_Speed;
	I2C_Init(I2C2,&I2C_InitStructure);	
	I2C_Cmd(I2C2,ENABLE);
}

uint8_t i2c_single_read(uint8_t slave,uint8_t addr)
{
	uint8_t dat=0;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C2,ENABLE);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2,slave,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C2,addr);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTART(I2C2,ENABLE);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2,slave,I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	I2C_AcknowledgeConfig(I2C2,DISABLE);
	I2C_GenerateSTOP(I2C2,ENABLE);	
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED));
	dat=I2C_ReceiveData(I2C2);	
	I2C_AcknowledgeConfig(I2C2,ENABLE);
	return dat;
}

void i2c_single_write(uint8_t slave,uint8_t addr)
{
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C2,ENABLE);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2,slave,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C2,addr);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(I2C2,ENABLE);	
}

uint16_t i2c_read(uint8_t slave,uint8_t addr)
{
	uint8_t msb,lsb;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C2,ENABLE);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2,slave,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C2,addr);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTART(I2C2,ENABLE);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2,slave,I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	I2C_AcknowledgeConfig(I2C2,ENABLE);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED));
	msb=I2C_ReceiveData(I2C2);	
	I2C_AcknowledgeConfig(I2C2,DISABLE);
	I2C_GenerateSTOP(I2C2,ENABLE);		
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED));
	lsb=I2C_ReceiveData(I2C2);

	return ((msb<<8)|lsb);
}

void i2c_write(uint8_t slave,uint8_t addr,uint8_t dat)
{
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C2,ENABLE);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2,slave,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C2,addr);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C2,dat);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(I2C2,ENABLE);	
}

void i2c_multi_read(uint8_t slave,uint8_t reg,uint8_t len,uint8_t *buf)
{
	uint8_t i;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C2,ENABLE);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2,slave,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C2,reg);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTART(I2C2,ENABLE);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2,slave,I2C_Direction_Receiver);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	
	for(i=0;i<len;i++){
		if(i==len-1)
			I2C_AcknowledgeConfig(I2C2,DISABLE);
		else
			I2C_AcknowledgeConfig(I2C2,ENABLE);
		while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED));
		buf[i]=I2C_ReceiveData(I2C2);		
	}
	I2C_GenerateSTOP(I2C2,ENABLE);
}

void i2c_multi_write(uint8_t slave,uint8_t reg,uint8_t len,uint8_t *buf)
{
	uint8_t i;
	while(I2C_GetFlagStatus(I2C2,I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C2,ENABLE);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C2,slave,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(I2C2,reg);
	while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	for(i=0;i<len;i++){
		I2C_SendData(I2C2,buf[i]);
		while(!I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	I2C_GenerateSTOP(I2C2,ENABLE);
}

#endif

#ifdef SOFT_I2C
void i2c_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB,&GPIO_InitStructure);	
}

static void i2c_dalay()
{
//	delay_us(1);
}

void i2c_start()
{
    SDA_H;
	SCL_H;
	i2c_dalay();
    SDA_L;
    i2c_dalay();
    SCL_L;
    i2c_dalay();
}

void i2c_stop()
{
	SCL_L;
	i2c_dalay();
	SDA_L;
    i2c_dalay();
    SCL_H;
    i2c_dalay();
    SDA_H;
    i2c_dalay();
}

void i2c_send_ack()
{
	SCL_L;
	i2c_dalay();
	SDA_L;
	i2c_dalay();
	SCL_H;
	i2c_dalay();
	SCL_L;
	i2c_dalay();
}

void i2c_send_noack()
{
	SCL_L;
	i2c_dalay();
	SDA_H;
	i2c_dalay();
	SCL_H;
	i2c_dalay();
	SCL_L;
	i2c_dalay();
}

uint8_t i2c_send_byte(uint8_t dat)
{
    uint8_t i,ack;
    for (i=0; i<8; i++){
        if((dat <<i) & 0x80)
            SDA_H;
        else
            SDA_L;
		SCL_H;
        i2c_dalay();
        SCL_L;
        i2c_dalay();
    }
    i2c_dalay();
	SCL_H;
    if(SDA_read)
        ack=0;
    else
        ack=1;
    i2c_dalay();
    SCL_L;
    i2c_dalay();
    return ack;
}

uint8_t i2c_recv_byte()
{
    uint8_t i;
    uint8_t dat = 0;
	SDA_H;
    for (i=0; i<8; i++){
        dat=dat<<1;		
        SCL_H;
        i2c_dalay();
        if(SDA_read)
            dat++;
		SCL_L;
        i2c_dalay();
    }
    return dat;
}

uint8_t i2c_single_read(uint8_t dev,uint8_t addr)
{
	uint8_t dat;
	i2c_start();
	i2c_send_byte(dev);
	i2c_send_byte(addr);
	i2c_start();
	i2c_send_byte(dev+1);
	dat=i2c_recv_byte();
	i2c_send_noack();
	i2c_stop();
	return dat;
}

void i2c_single_write(uint8_t dev,uint8_t addr)
{
    i2c_start();
    i2c_send_byte(dev);
    i2c_send_byte(addr);
    i2c_stop();
}

uint16_t i2c_read(uint8_t dev,uint8_t addr)
{
	uint8_t msb,lsb;
	uint16_t dat;
	i2c_start();
	i2c_send_byte(dev);
	i2c_send_byte(addr);
	i2c_start();
	i2c_send_byte(dev+1);
	msb=i2c_recv_byte();
	i2c_send_ack();
	lsb=i2c_recv_byte();
	i2c_send_noack();
	i2c_stop();
	dat=(msb<<8)+lsb;
	return dat;
}
 
void i2c_write(uint8_t dev,uint8_t addr,uint8_t dat)
{
    i2c_start();
    i2c_send_byte(dev);
    i2c_send_byte(addr);
    i2c_send_byte(dat);
    i2c_stop();
}

void i2c_multi_read(uint8_t dev,uint8_t reg,uint8_t len,uint8_t *buf)
{
	unsigned char i;
	i2c_start();
	i2c_send_byte(dev);
	i2c_send_byte(reg);
	i2c_start();
	i2c_send_byte(dev+1);
	for(i=0;i<len;i++){
		buf[i]=i2c_recv_byte();
		if(i==len-1)
			i2c_send_noack();
		else
			i2c_send_ack();
	}
	i2c_stop();
}

void i2c_multi_write(uint8_t dev,uint8_t reg,uint8_t len,uint8_t *buf)
{
	unsigned char i;
	i2c_start();
	i2c_send_byte(dev);
	i2c_send_byte(reg);
	for(i=0;i<len;i++){
		i2c_send_byte(buf[i]);
	}
	i2c_stop();
}

#endif
