/**
  ******************************************************************************
  * @file    usbd_customhid.h
  * @author  MCD Application Team
  * @brief   header file for the usbd_customhid.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CS_CONTROL_H
#define __USB_CS_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_CONTROL
  * @brief This file is the Header file for USBD_customhid.c
  * @{
  */


/** @defgroup USBD_CONTROL_Exported_Defines
  * @{
  */

#define USB_CONTROL_CONFIG_DESC_SIZ       	18U
#define USB_CONTROL_DESC_SIZ              	9U

#ifndef CONTROL_FS_BINTERVAL
#define CONTROL_FS_BINTERVAL            	0x05U
#endif /* CONTROL_FS_BINTERVAL */

#define CS_CONTROL_POWER_CTRL 				0x31
#define CS_CONTROL_CALIB_MODE_CTRL			0x32
#define CS_CONTROL_SAVE_CALIB_DATA			0x33
#define CS_CONTROL_SET_RAW_OFFSET			0x34
#define CS_CONTROL_SET_RAW_AMPL				0x35
#define CS_CONTROL_SET_AMPL					0x36
/**
  * @}
  */


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */
typedef struct
{
  uint32_t             AltSetting;
}
USBD_CONTROL_HandleTypeDef;
/**
  * @}
  */



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */

extern USBD_ClassTypeDef  USBD_CONTROL;
#define USBD_CONTROL_CLASS    &USBD_CONTROL
/**
  * @}
  */

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /* __USB_CUSTOMHID_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
