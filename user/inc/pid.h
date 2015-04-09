#ifndef __PID_H__
#define __PID_H__

typedef struct _PID
{
	float P,pout,I,iout,D,dout,IMAX,OUT;
}PID;

void pid_init(void);
void pid_control(void);

#endif
