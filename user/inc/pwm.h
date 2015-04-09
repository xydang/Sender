#ifndef __PWM_H__
#define __PWM_H__


#define MIN_THROTTLE 1000
#define MAX_THROTTLE 1900
#define UNLOCK_THROTTLE 900

extern int16_t MOTO1_PWM;
extern int16_t MOTO2_PWM;
extern int16_t MOTO3_PWM;
extern int16_t MOTO4_PWM;
extern int16_t MOTO5_PWM;
extern int16_t MOTO6_PWM;
extern int16_t MOTO_PWM_MAX;
extern int16_t MOTO_PWM_MIN;

extern uint8_t unlock_flag;

void pwm_init(void);
void pwm_set(int16_t motor1,int16_t motor2,int16_t motor3,int16_t motor4,int16_t motor5,int16_t motor6);

#endif

