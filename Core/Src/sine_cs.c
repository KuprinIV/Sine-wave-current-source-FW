#include "sine_cs.h"
#include "sine_array.h"
#include "main.h"
#include <math.h>
#include <string.h>

// driver functions
static void init(void);
static void powerControl(uint8_t is_enabled);
static void setSineRawAmplitude(uint16_t dac_ampl);
static void setSineAmplitude(uint8_t ampl);
static void setSineOffset(uint16_t dac_offset);
static void calibrationModeControl(uint8_t is_enabled);
static void saveCalibrationData(void);

// inner functions
static void calcHalfSineWave(uint16_t amplitude, uint16_t offset);

uint16_t sineHalfPeriod[SINE_SAMPLES_NUM] = {0};
uint16_t tempBuf[SINE_SAMPLES_NUM] = {0};

volatile uint8_t isFullSineParamsChanged = 0;
volatile uint8_t isHalfSineParamsChanged = 0;
volatile uint8_t isCalibrationModeEnabled = 0;

volatile uint16_t sineAmplitude = 124;
volatile uint16_t sineAmplitude_1A = 124;
volatile uint16_t sineOffset = 372;

sineCS_driver sineCS = {
		init,
		powerControl,
		setSineRawAmplitude,
		setSineAmplitude,
		setSineOffset,
		calibrationModeControl,
		saveCalibrationData,
};

sineCS_driver* sineCS_drv = &sineCS;

/**
  * @brief  Sine CS initialization: read calibration values from EEPROM
  * @param  None
  * @retval None
  */
static void init(void)
{
	// read calibration data, if it was saved
	if((*(__IO uint32_t *)EEPROM_CAL_DATA_ADDR) != 0)
	{
		sineAmplitude_1A = (*(__IO uint16_t *)EEPROM_CAL_DATA_ADDR);
		sineAmplitude = sineAmplitude_1A;

		sineOffset = (*(__IO uint16_t *)(EEPROM_CAL_DATA_ADDR+2));
	}
}

/**
  * @brief  Sine CS power control
  * @param  is_enabled: 0 - power off, 1 - power on
  * @retval None
  */
static void powerControl(uint8_t is_enabled)
{
	if(is_enabled)
	{
		DC_EN_GPIO_Port->ODR |= DC_EN_Pin;
		// LED indication
		LED_GPIO_Port->ODR |= LED_Pin;
		// calc sine wave for DAC
		calcHalfSineWave(sineAmplitude, sineOffset);
	}
	else
	{
		DC_EN_GPIO_Port->ODR &= ~DC_EN_Pin;
		// LED indication
		LED_GPIO_Port->ODR &= ~LED_Pin;
		// set DAC output to zero
		calcHalfSineWave(0, 0);
	}
}

/**
  * @brief  Set sine amplitude in DAC discretes. Used for 1A amplitude calibration
  * @param  dac_ampl: 0...4095 - DAC value
  * @retval None
  */
static void setSineRawAmplitude(uint16_t dac_ampl)
{
	if(isCalibrationModeEnabled)
	{
		// limit amplitude value
		if(dac_ampl < 50) dac_ampl = 50;
		if(dac_ampl > 250) dac_ampl = 250;

		sineAmplitude = dac_ampl;
		calcHalfSineWave(sineAmplitude, sineOffset);
	}
}

/**
  * @brief  Set sine amplitude in 0,1 A discretes. Used for setting current amplitude
  * @param  ampl: 0...70 - current amplitude from 0 to 7,0 A with 0,1 A step
  * @retval None
  */
static void setSineAmplitude(uint8_t ampl)
{
	uint32_t temp = 0;
	if(!isCalibrationModeEnabled)
	{
		if(ampl > 70) ampl = 70; // limit value by 7A
		temp = (uint32_t)(ampl*sineAmplitude_1A);
		sineAmplitude = temp/10;
		calcHalfSineWave(sineAmplitude, sineOffset);
	}
}

/**
  * @brief  Set sine offset in DAC discretes. Used in calibration mode for minimizing step after switching polarity
  * @param  offset: 0...4095 - sine offset DAC value
  * @retval None
  */
static void setSineOffset(uint16_t offset)
{
	if(isCalibrationModeEnabled)
	{
		// limit offset value
		if(offset < 250) offset = 250;
		if(offset > 500) offset = 500;

		sineOffset = offset;
		calcHalfSineWave(sineAmplitude, sineOffset);
	}
}

/**
  * @brief  Sine CS calibration mode control
  * @param  is_enabled: 0 - exit from calibration mode, 1 - enter into calibration mode
  * @retval None
  */
static void calibrationModeControl(uint8_t is_enabled)
{
	isCalibrationModeEnabled = is_enabled;
}

/**
  * @brief  Save calibration values in EEPROM
  * @param  None
  * @retval None
  */
static void saveCalibrationData(void)
{
	HAL_StatusTypeDef flash_ok = HAL_ERROR;

	if(isCalibrationModeEnabled)
	{
		// update 1A DAC value
		sineAmplitude_1A = sineAmplitude;

		HAL_FLASHEx_DATAEEPROM_Unlock();

		flash_ok = HAL_FLASHEx_DATAEEPROM_Erase(EEPROM_CAL_DATA_ADDR);
		if(flash_ok == HAL_OK)
		{
			flash_ok = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_HALFWORD, EEPROM_CAL_DATA_ADDR, sineAmplitude_1A);
			flash_ok = HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_HALFWORD, EEPROM_CAL_DATA_ADDR+2, sineOffset);
			if(flash_ok != HAL_OK)
			{
				Error_Handler();
			}
		}

		HAL_FLASHEx_DATAEEPROM_Lock();
	}
}

/**
  * @brief  Calculate half sine wave period with given amplitude and offset in DAC discretes
  * @param  amplitude: 0...4095 - sine wave amplitude in DAC discretes
  * @param  offset: 0...4095 - sine wave offset in DAC discretes
  * @retval None
  */
static void calcHalfSineWave(uint16_t amplitude, uint16_t offset)
{
  uint32_t temp = 0;
  for(uint16_t i = 0; i < SINE_SAMPLES_NUM; i++)
  {
	  temp = (uint32_t)(amplitude*sineArray[i]);
	  tempBuf[i] = (uint16_t)(temp>>12) + offset;
  }
  isHalfSineParamsChanged = 1;
  isFullSineParamsChanged = 1;
}

// update DAC data buffer after changing sine wave parameters
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
	uint16_t bufferSize = sizeof(sineHalfPeriod)/2;
	if(isHalfSineParamsChanged)
	{
		isHalfSineParamsChanged = 0;
		memcpy(sineHalfPeriod, tempBuf, bufferSize);
	}
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
	uint16_t bufferSize = sizeof(sineHalfPeriod)/2;
	if(isFullSineParamsChanged)
	{
		isFullSineParamsChanged = 0;
		memcpy(sineHalfPeriod+SINE_SAMPLES_NUM/2, tempBuf+SINE_SAMPLES_NUM/2, bufferSize);
	}
}
