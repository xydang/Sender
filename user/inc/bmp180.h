#ifndef __BMP180_H__
#define __BMP180_H__

#define	BMP180_ADDR	0xee // 8bit
//	#define	BMP180_ADDR	0x77 // 7bit

#define OSS 0
#define BMP180_FILTER_NUM 10

#define BMP180_PROM_START_ADDR         0xaa
#define BMP180_PROM_DATA_LEN           22

#define BMP180_CHIP_ID_REG             0xD0
#define BMP180_VERSION_REG             0xD1

#define BMP180_CTRL_MEAS_REG           0xF4
#define BMP180_ADC_OUT_MSB_REG         0xF6
#define BMP180_ADC_OUT_LSB_REG         0xF7

#define BMP180_SOFT_RESET_REG          0xE0

#define BMP180_T_MEASURE               0x2E
#define BMP180_P_MEASURE               0x34

#define PARAM_MG      3038        /*calibration parameter */
#define PARAM_MH     -7357        /*calibration parameter */
#define PARAM_MI      3791        /*calibration parameter */

typedef struct bmp180_calibration_t{
	int16_t ac1;
	int16_t ac2; 
	int16_t ac3; 
	uint16_t ac4;
	uint16_t ac5;
	uint16_t ac6;
	int16_t b1; 
	int16_t b2;
	int16_t mb;
	int16_t mc;
	int16_t md;
}BMP180_Calibration;

typedef struct bmp180_t{
	int16_t temp;
	uint32_t pressure;
	int16_t altitude;
}BMP180_Def;

void bmp180_init(void);
int bmp180_read(BMP180_Def *bmp180);
void bmp180_filter(BMP180_Def *bmp180);


#endif
