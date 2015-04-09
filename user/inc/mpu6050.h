#ifndef __MPU6050_H__
#define __MPU6050_H__

#define	MPU6050_ADDR	0xd0 // 8bit
//	#define	MPU6050_ADDR	0x68 // 7bit

#define	SMPLRT_DIV		0x19
#define	CONFIG			0x1A
#define I2C_MST_CTRL    0x24

#define	GYRO_CONFIG		0x1B
#define	ACCEL_CONFIG	0x1C

#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40

#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42

#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48

#define	PWR_MGMT_1		0x6B
#define	WHO_AM_I		0x75

#define MPU6050_FILTER_NUM	10

typedef struct mpu6050_t{
	int16_t gx;
	int16_t gy;
	int16_t gz;
	int16_t ax;
	int16_t ay;
	int16_t az;
}MPU6050_Def;

void mpu6050_init(void);
void mpu6050_read(MPU6050_Def *gyro);
void mpu6050_filter(MPU6050_Def *gyro);
void mpu6050_deal(MPU6050_Def *gyro);

#endif


