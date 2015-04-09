#ifndef __I2C_H__
#define __I2C_H__

#define I2C_Speed		400000

#define SCL_H         GPIOB->BSRR = GPIO_Pin_10
#define SCL_L         GPIOB->BRR  = GPIO_Pin_10 
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_11
#define SDA_L         GPIOB->BRR  = GPIO_Pin_11

#define SCL_read      GPIOB->IDR  & GPIO_Pin_10
#define SDA_read      GPIOB->IDR  & GPIO_Pin_11

void i2c_init(void);
uint8_t i2c_single_read(uint8_t slave,uint8_t addr);
void i2c_single_write(uint8_t slave,uint8_t addr);
uint16_t i2c_read(uint8_t slave,uint8_t addr);
void i2c_write(uint8_t slave,uint8_t addr,uint8_t dat);
void i2c_multi_read(uint8_t slave,uint8_t reg,uint8_t len,uint8_t *buf);
void i2c_multi_write(uint8_t slave,uint8_t reg,uint8_t len,uint8_t *buf);


#endif
