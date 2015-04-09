#include <stdint.h>
#include <math.h>
#include "i2c.h"
#include "bmp180.h"
#include "delay.h"

BMP180_Calibration bmp180_cali;
int32_t alt_buf[BMP180_FILTER_NUM];
uint32_t press_buf[BMP180_FILTER_NUM];
uint8_t bmp180_state=0;

void bmp180_init(void)
{
	uint8_t data[BMP180_PROM_DATA_LEN];
	i2c_multi_read(BMP180_ADDR,BMP180_PROM_START_ADDR,BMP180_PROM_DATA_LEN,data);
	bmp180_cali.ac1 = (data[0] << 8) | data[1];
	bmp180_cali.ac2 =  (data[2] << 8) | data[3];
	bmp180_cali.ac3 =  (data[4] << 8) | data[5];
	bmp180_cali.ac4 =  (data[6] << 8) | data[7];
	bmp180_cali.ac5 =  (data[8] << 8) | data[9];
	bmp180_cali.ac6 = (data[10] << 8) | data[11];
	bmp180_cali.b1 =  (data[12] << 8) | data[13];
	bmp180_cali.b2 =  (data[14] << 8) | data[15];
	bmp180_cali.mb =  (data[16] << 8) | data[17];
	bmp180_cali.mc =  (data[18] << 8) | data[19];
	bmp180_cali.md =  (data[20] << 8) | data[21];
	
	/*
	bmp180_cali.ac1 = i2c_read(BMP180_ADDR,0xAA);
	bmp180_cali.ac2 = i2c_read(BMP180_ADDR,0xAC);
	bmp180_cali.ac3 = i2c_read(BMP180_ADDR,0xAE);
	bmp180_cali.ac4 = i2c_read(BMP180_ADDR,0xB0);
	bmp180_cali.ac5 = i2c_read(BMP180_ADDR,0xB2);
	bmp180_cali.ac6 = i2c_read(BMP180_ADDR,0xB4);
	bmp180_cali.b1 =  i2c_read(BMP180_ADDR,0xB6);
	bmp180_cali.b2 =  i2c_read(BMP180_ADDR,0xB8);
	bmp180_cali.mb =  i2c_read(BMP180_ADDR,0xBA);
	bmp180_cali.mc =  i2c_read(BMP180_ADDR,0xBC);
	bmp180_cali.md =  i2c_read(BMP180_ADDR,0xBE);
	*/
	
}


int bmp180_read(BMP180_Def *bmp180)
{
	float alt;
	int32_t up,x1,x2,b5,b6,x3,b3,p;
	static int32_t ut;
	uint32_t b4, b7;
	if(bmp180_state==0){
		bmp180_state++;
		i2c_write(BMP180_ADDR,BMP180_CTRL_MEAS_REG,BMP180_T_MEASURE);
		return -1;
	}
	if(bmp180_state==1){
		bmp180_state++;
		ut=i2c_read(BMP180_ADDR,BMP180_ADC_OUT_MSB_REG);
		i2c_write(BMP180_ADDR,BMP180_CTRL_MEAS_REG,BMP180_P_MEASURE);
		return -1;
	}
	if(bmp180_state==2)
		bmp180_state=0;
	up=i2c_read(BMP180_ADDR,BMP180_ADC_OUT_MSB_REG);
	x1 = (((int32_t)ut-(int32_t)bmp180_cali.ac6)*(int32_t)bmp180_cali.ac5)>>15;
	x2 = ((int32_t)bmp180_cali.mc <<11)/(x1+bmp180_cali.md);
	b5 = x1 + x2;
	bmp180->temp=((b5 + 8) >> 4);
	b6 = b5 - 4000;
	/*****calculate B3************/
	x1 = (bmp180_cali.b2 * (b6 * b6)>>12)>>11;
	x2 = (bmp180_cali.ac2 * b6)>>11;
	x3 = x1 + x2;
	b3 = (((((int32_t)bmp180_cali.ac1)*4 + x3)<<OSS) + 2)>>2;
	/*****calculate B4************/
	x1 = (bmp180_cali.ac3 * b6)>>13;
	x2 = (bmp180_cali.b1 * ((b6 * b6)>>12))>>16;
	x3 = ((x1 + x2) + 2)>>2;
	b4 = (bmp180_cali.ac4 * (uint32_t)(x3 + 32768))>>15;
	
	b7 = ((uint32_t)(up - b3) * (50000>>OSS));
	if (b7 < 0x80000000)
		p = (b7<<1)/b4;
	else
		p = (b7/b4)<<1;
	x1 = p>>8;
	x1 *=x1;
	x1 = (x1 * PARAM_MG)>>16;
	x2 = (PARAM_MH * p)>>16;
	bmp180->pressure=p+((x1+x2+PARAM_MI)>>4);
	alt=44330.76*(1-pow((float)bmp180->pressure/101325,0.190263));
	alt*=10;
	bmp180->altitude=alt;
	return 0;
}

void bmp180_filter(BMP180_Def *bmp180)
{
	static uint8_t filter_cnt=0;
	uint8_t i;
	uint32_t press_sum=0;
	int32_t alt_sum=0;
	press_buf[filter_cnt] =bmp180->pressure;
	alt_buf[filter_cnt]=bmp180->altitude;
	for(i=0;i<BMP180_FILTER_NUM;i++){
		press_sum+= press_buf[i];
		alt_sum+=alt_buf[i];
	}
	bmp180->pressure=press_sum/BMP180_FILTER_NUM;
	bmp180->altitude=alt_sum/BMP180_FILTER_NUM;
	filter_cnt++;
	if(filter_cnt==BMP180_FILTER_NUM)
		filter_cnt=0;
}





