#include <stdint.h>
#include "pid.h"
#include "imu.h"
#include "pwm_in.h"
#include "pwm.h"

PID PID_ROL,PID_PIT,PID_YAW,PID_ALT,PID_POS;

int16_t getlast_roll=0,geilast_pitch=0;
float rol_i=0,pit_i=0,yaw_p=0;

void pid_init(void)
{
	PID_ROL.P=900;
	PID_ROL.I=8;
	PID_ROL.D=10;
	
	PID_PIT.P=900;
	PID_PIT.I=8;
	PID_PIT.D=10;
	
	PID_YAW.P=1000;
	PID_YAW.I=0;
	PID_YAW.D=10;
	
}

void pid_control(void)
{
	EULER_Def angle;
	angle.roll=euler.roll-(rc_in.roll-1500)/12;
	angle.pitch=euler.pitch-(rc_in.pitch-1500)/12;
	
	rol_i += angle.roll;
	
	if(rol_i>2000)
		rol_i=2000;
	if(rol_i<-2000)
		rol_i=-2000;
	
	PID_ROL.pout = PID_ROL.P * angle.roll;
	PID_ROL.dout = -PID_ROL.D * imu.gy;//attention!!!
	PID_ROL.iout = PID_ROL.I * PID_ROL.dout;	
	
	pit_i += angle.pitch;
	if(pit_i>2000)
		pit_i=2000;
	if(pit_i<-2000)
		pit_i=-2000;
	
	PID_PIT.pout = PID_PIT.P * angle.pitch;
	PID_PIT.dout = PID_PIT.D * imu.gx;
	PID_PIT.iout = PID_PIT.I * pit_i;
	
	if(rc_in.yaw<1400||rc_in.yaw>1600){
		imu.gz=imu.gz+(rc_in.yaw-1500)*2;
	}
	yaw_p+=imu.gz*0.0609756f*0.002f;// +(Rc_Get.YAW-1500)*30
	if(yaw_p>20)
		yaw_p=20;
	if(yaw_p<-20)
		yaw_p=-20;
	
	PID_YAW.pout=PID_YAW.P*yaw_p;
	PID_YAW.dout = PID_YAW.D * imu.gz;

	if(rc_in.thr<1200)
	{		
		pit_i=0;
		rol_i=0;
		yaw_p=0;
	}
	
	PID_ROL.OUT =  (-PID_ROL.pout)-PID_ROL.iout +PID_ROL.dout;
	PID_PIT.OUT = PID_PIT.pout + PID_PIT.iout + PID_PIT.dout;
	PID_YAW.OUT = PID_YAW.pout + PID_YAW.iout + PID_YAW.dout;
	
	if((rc_in.thr>1200)&&(rc_in.armed==1))
	{
		MOTO1_PWM = rc_in.thr - 1000 + PID_ROL.OUT - PID_PIT.OUT - PID_YAW.OUT;
		MOTO2_PWM = rc_in.thr - 1000 + PID_ROL.OUT + PID_PIT.OUT + PID_YAW.OUT;
		MOTO3_PWM = rc_in.thr - 1000 - PID_ROL.OUT + PID_PIT.OUT - PID_YAW.OUT;
		MOTO4_PWM = rc_in.thr - 1000 - PID_ROL.OUT - PID_PIT.OUT + PID_YAW.OUT;
	}else{
		MOTO1_PWM=0;
		MOTO2_PWM=0;
		MOTO3_PWM=0;
		MOTO4_PWM=0;
	}
	
	pwm_set(MOTO1_PWM,MOTO2_PWM,MOTO3_PWM,MOTO4_PWM,0,0);
}

