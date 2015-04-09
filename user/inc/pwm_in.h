#ifndef __PWM_IN_H__
#define __PWM_IN_H__

void pwm_in_init(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);

typedef struct _pwm_in {
    unsigned char state;
    int rise;
    int fall;
    int capture;
}PWM_Input;

typedef struct _rc_in{
	int16_t thr;
	int16_t roll;
	int16_t pitch;
	int16_t yaw;
	int16_t mode;
	int16_t serve;
	int16_t armed;
}RC_Input;
extern RC_Input rc_in;
#endif
