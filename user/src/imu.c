#include <stdint.h>
#include <math.h>
#include "imu.h"
#include "mpu6050.h"
#include "hmc5883.h"
#include "bmp180.h"

IMU_Def imu;
QUAT_Def quat;
EULER_Def euler;


void quat_init(IMU_Def *imu,QUAT_Def *quat,EULER_Def *euler)
{
	EULER_Def euler_init;
	double norm;
	norm=sqrt(imu->ax*imu->ax+imu->ay*imu->ay+imu->az*imu->az);
	imu->ax/=norm;
	imu->ay/=norm;
	imu->az/=norm;
	
	euler_init.roll=atan2(imu->ay,imu->az);
	euler_init.pitch=atan2(imu->ax,imu->az);
	euler_init.roll/=2.0;
	euler_init.pitch/=2.0;
	euler_init.yaw=0;
	
//	euler->yaw=atan2(imu->ax*imu->mz-imu->az*imu->mx,imu->az*imu->my-imu->ay*imu->mz)+PI/2+0.095993108;
	
	quat->q0=cos(euler_init.yaw)*cos(euler_init.pitch)*cos(euler_init.roll)+sin(euler_init.yaw)*sin(euler_init.pitch)*sin(euler_init.roll);
	quat->q1=cos(euler_init.yaw)*cos(euler_init.pitch)*sin(euler_init.roll)-sin(euler_init.yaw)*sin(euler_init.pitch)*cos(euler_init.roll);
	quat->q2=cos(euler_init.yaw)*sin(euler_init.pitch)*cos(euler_init.roll)+sin(euler_init.yaw)*cos(euler_init.pitch)*sin(euler_init.roll);
	quat->q3=sin(euler_init.yaw)*cos(euler_init.pitch)*cos(euler_init.roll)-cos(euler_init.yaw)*sin(euler_init.pitch)*sin(euler_init.roll);
	
	norm = sqrt(quat->q0*quat->q0 + quat->q1*quat->q1 + quat->q2*quat->q2 + quat->q3*quat->q3);
	quat->q0 = quat->q0 / norm;
	quat->q1 = quat->q1 / norm;
	quat->q2 = quat->q2 / norm;
	quat->q3 = quat->q3 / norm;
}

void imu_read(IMU_Def *imu)
{
	MPU6050_Def mpu;
	HMC5883_Def hmc;

	mpu6050_read(&mpu);
	mpu6050_filter(&mpu);
	mpu6050_deal(&mpu);
	hmc5883_read(&hmc);

	imu->ax=mpu.ax;
	imu->ay=mpu.ay;
	imu->az=mpu.az;
	imu->gx=mpu.gx;
	imu->gy=mpu.gy;
	imu->gz=mpu.gz;
	imu->mx=hmc.mx;
	imu->my=hmc.my;
	imu->mz=hmc.mz;
	imu->heading=hmc.heading;
}

void imu_update(IMU_Def *imu,QUAT_Def *quat,EULER_Def *euler)
{
	double gx,gy,gz,ax,ay,az;	
	float vx, vy, vz;
	float ex, ey, ez;
	float q0q0,q0q1,q0q2,q0q3,q1q1,q1q2,q1q3,q2q2,q2q3,q3q3;
	static float exInt = 0, eyInt = 0, ezInt=0;
	double t[3][3]; 
	//double yaw_mag;
	double norm;
	ax=imu->ax;
	ay=imu->ay;
	az=imu->az;
	gx=imu->gx;
	gy=imu->gy;
	gz=imu->gz;
	
	q0q0=quat->q0*quat->q0;
	q0q1=quat->q0*quat->q1;
	q0q2=quat->q0*quat->q2;
	q0q3=quat->q0*quat->q3;
	q1q1=quat->q1*quat->q1;
	q1q2=quat->q1*quat->q2;
	q1q3=quat->q1*quat->q3;
	q2q2=quat->q2*quat->q2;
	q2q3=quat->q2*quat->q3;
	q3q3=quat->q3*quat->q3;
	
	norm = sqrt(ax*ax + ay*ay + az*az);
	ax = ax /norm;
	ay = ay / norm;
	az = az / norm;

	vx = 2*(q1q3 - q0q2);
	vy = 2*(q0q1 + q2q3);
	vz = q0q0 - q1q1 - q2q2 + q3q3 ;

	ex = (ay*vz - az*vy) ;
	ey = (az*vx - ax*vz) ;
	ez = (ax*vy - ay*vx) ;
	
	exInt = exInt + ex * Ki;
	eyInt = eyInt + ey * Ki;
	ezInt = ezInt + ez * Ki;

	gx = gx + Kp*ex + exInt;
	gy = gy + Kp*ey + eyInt;
	gz = gz + Kp*ez + ezInt;
	
	quat->q0 = quat->q0 + (-quat->q1*gx - quat->q2*gy - quat->q3*gz)*halfT;
	quat->q1 = quat->q1 + ( quat->q0*gx + quat->q2*gz - quat->q3*gy)*halfT;
	quat->q2 = quat->q2 + ( quat->q0*gy - quat->q1*gz + quat->q3*gx)*halfT;
	quat->q3 = quat->q3 + ( quat->q0*gz + quat->q1*gy - quat->q2*gx)*halfT;
	
	norm = sqrt(quat->q0*quat->q0 + quat->q1*quat->q1 + quat->q2*quat->q2 + quat->q3*quat->q3);
	quat->q0 = quat->q0 / norm;
	quat->q1 = quat->q1 / norm;
	quat->q2 = quat->q2 / norm;
	quat->q3 = quat->q3 / norm;

	q0q0=quat->q0*quat->q0;
	q0q1=quat->q0*quat->q1;
	q0q2=quat->q0*quat->q2;
	q0q3=quat->q0*quat->q3;
	q1q1=quat->q1*quat->q1;
	q1q2=quat->q1*quat->q2;
	q1q3=quat->q1*quat->q3;
	q2q2=quat->q2*quat->q2;
	q2q3=quat->q2*quat->q3;
	q3q3=quat->q3*quat->q3;

	t[1][2]=2.0*(q2q3+q0q1);
	t[2][2]=q0q0-q1q1-q2q2+q3q3;
	t[0][2]=2.0*(q1q3-q0q2);
	t[0][1]=2.0*(q1q2+q0q3);
	t[0][0]=q0q0+q1q1-q2q2-q3q3;

	euler->pitch = -atan2(t[1][2],t[2][2])*R2D;
	euler->roll = -asin(t[0][2])*R2D;
	euler->yaw =(2*PI-atan2(t[0][1],t[0][0]))*R2D;
//	euler->yaw += imu->gz*0.0610351f*0.002f;
	if(euler->yaw>360)
		euler->yaw-=360;
	if(euler->yaw<0)
		euler->yaw+=360;
}



