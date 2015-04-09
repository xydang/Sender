#ifndef __IMU_H__
#define __IMU_H__

#define PI  3.141592654f
#define PPI 6.283185307f

#define Kp 1.6f
#define Ki 0.001f
#define halfT 0.002f
#define gyro_denominator 1834 // 65.5*28=1834 


#define PI  3.141592654f
#define PPI 6.283185307f
#define R2D 57.29577951
#define GLSB 65.5
#define ALSB 8192

typedef struct _euler{
	double roll;
	double pitch;
	double yaw;
}EULER_Def;

typedef struct _quat{
	double q0;
	double q1;
	double q2;
	double q3;
}QUAT_Def;

typedef struct _imu{
	int16_t gx;
	int16_t gy;
	int16_t gz;
	int16_t ax;
	int16_t ay;
	int16_t az;
	int16_t mx;
	int16_t my;
	int16_t mz;
	int16_t alt;
	int16_t alt_init;
	int16_t heading;
	uint16_t press;
	uint16_t temp;
}IMU_Def;

void quat_init(IMU_Def *imu,QUAT_Def *quat,EULER_Def *euler);
void imu_read(IMU_Def *imu);
void imu_update(IMU_Def *imu,QUAT_Def *quat,EULER_Def *euler);

extern IMU_Def imu;
extern QUAT_Def quat;
extern EULER_Def euler;

#endif

