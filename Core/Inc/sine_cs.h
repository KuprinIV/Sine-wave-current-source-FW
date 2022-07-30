#ifndef __SINE_CS_H
#define __SINE_CS_H

#include "stm32l0xx_hal.h"

#define SINE_SAMPLES_NUM 500
#define EEPROM_CAL_DATA_ADDR 0x08080000

typedef struct
{
	void (*Init)(void);
	void (*PowerCtrl)(uint8_t is_enabled);
	void (*SetRawAmplitude)(uint16_t dac_value);
	void (*SetAmplitude)(uint8_t ampl);
	void (*SetRawOffset)(uint16_t dac_offset);
	void (*CalibrationModeCtrl)(uint8_t is_enabled);
	void (*SaveCalibrationData)(void);
}sineCS_driver;

extern sineCS_driver* sineCS_drv;

#endif
