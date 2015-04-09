#ifndef __HMC5883_H__
#define __HMC5883_H__


#define	HMC5883_ADDR	0x3c // 8bit
//	#define	HMC5883_ADDR	0x1e // 7bit


#define HMC5883_REG  0x03 

#define HMC5883L_OFFSET_X   (0)
#define HMC5883L_OFFSET_Y   (0)

#define HMC5883L_GAIN_X   1
#define HMC5883L_GAIN_Y   1

typedef struct hmc5883_t{
	int16_t mx;
	int16_t my;
	int16_t mz;
	float heading;
}HMC5883_Def;

void hmc5883_init(void);
void hmc5883_read(HMC5883_Def* compass);

#endif


