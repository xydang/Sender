#include <stdint.h>
#include "i2c.h"
#include "mpu6050.h"

int16_t acc_x_buf[MPU6050_FILTER_NUM],acc_y_buf[MPU6050_FILTER_NUM],acc_z_buf[MPU6050_FILTER_NUM];

void mpu6050_init(void)
{
	i2c_write(MPU6050_ADDR,PWR_MGMT_1,0x00);
	i2c_write(MPU6050_ADDR,SMPLRT_DIV,0x00);
	i2c_write(MPU6050_ADDR,CONFIG,0x03);
	i2c_write(MPU6050_ADDR,I2C_MST_CTRL,0x0D);
	i2c_write(MPU6050_ADDR,0x6A,0xC8);
	i2c_write(MPU6050_ADDR,0x37,0x32);
	i2c_write(MPU6050_ADDR,ACCEL_CONFIG,0x08);
	i2c_write(MPU6050_ADDR,GYRO_CONFIG,0x08);
}

void mpu6050_read(MPU6050_Def *gyro)
{
	gyro->gx=i2c_read(MPU6050_ADDR,GYRO_XOUT_H);
	gyro->gy=i2c_read(MPU6050_ADDR,GYRO_YOUT_H);
	gyro->gz=i2c_read(MPU6050_ADDR,GYRO_ZOUT_H);
	gyro->ax=i2c_read(MPU6050_ADDR,ACCEL_XOUT_H);
	gyro->ay=i2c_read(MPU6050_ADDR,ACCEL_YOUT_H);
	gyro->az=i2c_read(MPU6050_ADDR,ACCEL_ZOUT_H);
}

void mpu6050_filter(MPU6050_Def *gyro)
{
	static uint8_t filter_cnt=0;
	int32_t sum_x=0,sum_y=0,sum_z=0;
	uint8_t i;
	acc_x_buf[filter_cnt]=gyro->ax;
	acc_y_buf[filter_cnt]=gyro->ay;
	acc_z_buf[filter_cnt]=gyro->az;
	for(i=0;i<MPU6050_FILTER_NUM;i++){
		sum_x += acc_x_buf[i];
		sum_y += acc_y_buf[i];
		sum_z += acc_z_buf[i];
	}
	gyro->ax=sum_x / MPU6050_FILTER_NUM;
	gyro->ay=sum_y / MPU6050_FILTER_NUM;
	gyro->az=sum_z / MPU6050_FILTER_NUM;
	filter_cnt++;
	if(filter_cnt==MPU6050_FILTER_NUM)
		filter_cnt=0;
}

void mpu6050_deal(MPU6050_Def *gyro)
{
	gyro->ax/=835.066;//   8.92/9.8=835.066
	gyro->ay/=835.066;
	gyro->az/=835.066;
	gyro->gx/=1834;
	gyro->gy/=1834;
	gyro->gz/=1834;
}

