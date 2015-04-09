#ifndef __DELAY_H__
#define __DELAY_H__

void systick_init(void);
void delay_ms(uint16_t t);
void delay_us(uint16_t t);

extern uint16_t cnt_control,cnt_imu,cnt_report;

#endif

