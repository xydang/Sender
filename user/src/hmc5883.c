
#include <stdint.h>
#include <math.h>
#include "i2c.h"
#include "hmc5883.h"

void hmc5883_init(void)
{
	i2c_write(HMC5883_ADDR,0x00,0x03<<5|0x06<<2|0x10);
	i2c_write(HMC5883_ADDR,0x01,0x40);
	i2c_write(HMC5883_ADDR,0x02,0x00);
}

void hmc5883_read(HMC5883_Def* compass)
{
//	compass->mx=(int16_t)i2c_read(HMC5883_ADDR,0x03)+200;
//	compass->mz=(int16_t)i2c_read(HMC5883_ADDR,0x05);
//	compass->my=(int16_t)i2c_read(HMC5883_ADDR,0x07)-150;

	uint8_t buf[6];
	i2c_multi_read(HMC5883_ADDR,HMC5883_REG,6,buf);
	compass->mx=((buf[0] << 8) | buf[1])+HMC5883L_OFFSET_X;
	compass->mz=(buf[2] << 8) | buf[3];
	compass->my=((buf[4] << 8) | buf[5])-HMC5883L_OFFSET_Y;
	
	compass->heading=atan2f((float)compass->my,(float)compass->mx)*(180/3.14159265)+180;
}


