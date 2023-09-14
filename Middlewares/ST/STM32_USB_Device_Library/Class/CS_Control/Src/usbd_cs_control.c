/**
  ******************************************************************************
  * @file    usbd_customhid.c
  * @author  MCD Application Team
  * @brief   This file provides the CONTROL core functions.
  *
  * @verbatim
  *
  *          ===================================================================
  *                                CONTROL Class  Description
  *          ===================================================================
  *           This module manages the CONTROL class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (CONTROL) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - Usage Page : Generic Desktop
  *             - Usage : Vendor
  *             - Collection : Application
  *
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *
  *
  *  @endverbatim
  *
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

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cs_control.h"
#include "usbd_ctlreq.h"
#include "sine_cs.h"


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_CONTROL
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_CONTROL_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_CONTROL_Private_Defines
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_CONTROL_Private_Macros
  * @{
  */
/**
  * @}
  */
/** @defgroup USBD_CONTROL_Private_FunctionPrototypes
  * @{
  */

static uint8_t  USBD_CONTROL_Setup(USBD_HandleTypeDef *pdev,
                                      USBD_SetupReqTypedef *req);

static uint8_t  *USBD_CONTROL_GetFSCfgDesc(uint16_t *length);

static uint8_t  *USBD_CONTROL_GetDeviceQualifierDesc(uint16_t *length);
/**
  * @}
  */

/** @defgroup USBD_CONTROL_Private_Variables
  * @{
  */

USBD_ClassTypeDef  USBD_CONTROL =
{
  NULL,
  NULL,
  USBD_CONTROL_Setup,
  NULL, /*EP0_TxSent*/
  NULL, /*EP0_RxReady*/ /* STATUS STAGE IN */
  NULL, /*DataIn*/
  NULL,
  NULL, /*SOF */
  NULL,
  NULL,
  NULL,
  USBD_CONTROL_GetFSCfgDesc,
  NULL,
  USBD_CONTROL_GetDeviceQualifierDesc,
};

/* USB CONTROL device FS Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CONTROL_CfgFSDesc[USB_CONTROL_CONFIG_DESC_SIZ] __ALIGN_END =
{
	0x09, /* bLength: Configuration Descriptor size */
	USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
	USB_CONTROL_CONFIG_DESC_SIZ,
	/* wTotalLength: Bytes returned */
	0x00,
	0x01,         /*bNumInterfaces: 1 interface*/
	0x01,         /*bConfigurationValue: Configuration value*/
	0x00,         /*iConfiguration: Index of string descriptor describing
	the configuration*/
	0xC0,         /*bmAttributes: bus powered */
	0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

  	0x09,   /* bLength: Interface Descriptor size */
	USB_DESC_TYPE_INTERFACE,   /* bDescriptorType */
	0x00,   /* bInterfaceNumber: Number of Interface */
	0x00,      /* bAlternateSetting: Alternate setting */
	0x00,   /* bNumEndpoints*/
	0xFF,   /* bInterfaceClass: Vendor Specific Class Code */
	0x00,   /* bInterfaceSubClass*/
	0x00,   /* nInterfaceProtocol*/
	USBD_IDX_INTERFACE_STR + 1U /* iInterface: Index of string descriptor */
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_CONTROL_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */

/** @defgroup USBD_CONTROL_Private_Functions
  * @{
  */

/**
  * @brief  USBD_CONTROL_Setup
  *         Handle the CONTROL specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_CONTROL_Setup(USBD_HandleTypeDef *pdev,
                                      USBD_SetupReqTypedef *req)
{
  USBD_CONTROL_HandleTypeDef *hcs = (USBD_CONTROL_HandleTypeDef *)pdev->pClassData;
  uint16_t status_info = 0U;
  uint8_t ret = USBD_OK;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_VENDOR :
    	// handle vendor requests with sine CS control commands
      switch (req->bRequest)
      {
        case CS_CONTROL_POWER_CTRL:
		  sineCS_drv->PowerCtrl((uint8_t)(req->wValue & 0x01));
		  USBD_CtlSendStatus(pdev);
          break;

        case CS_CONTROL_CALIB_MODE_CTRL:
          sineCS_drv->CalibrationModeCtrl((uint8_t)(req->wValue & 0x01));
          USBD_CtlSendStatus(pdev);
          break;

        case CS_CONTROL_SAVE_CALIB_DATA:
        	sineCS_drv->SaveCalibrationData();
        	USBD_CtlSendStatus(pdev);
          break;

        case CS_CONTROL_SET_RAW_OFFSET:
        	sineCS_drv->SetRawOffset(req->wValue);
        	USBD_CtlSendStatus(pdev);
          break;

        case CS_CONTROL_SET_RAW_AMPL:
        	sineCS_drv->SetRawAmplitude(req->wValue);
        	USBD_CtlSendStatus(pdev);
          break;

        case CS_CONTROL_SET_AMPL:
        	sineCS_drv->SetAmplitude((uint8_t)(req->wValue & 0xFF));
        	USBD_CtlSendStatus(pdev);
          break;

        default:
          // skip 0x55 request
          if (req->bmRequest == 0xC0 && req->bRequest == 0x55) return ret;
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_GET_STATUS:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            USBD_CtlSendData(pdev, (uint8_t *)(void *)&status_info, 2U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_GET_DESCRIPTOR:

          break;

        case USB_REQ_GET_INTERFACE :
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            USBD_CtlSendData(pdev, (uint8_t *)(void *)&hcs->AltSetting, 1U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_SET_INTERFACE :
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            hcs->AltSetting = (uint8_t)(req->wValue);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }
  return ret;
}

/**
  * @brief  USBD_CONTROL_GetFSCfgDesc
  *         return FS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_CONTROL_GetFSCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_CONTROL_CfgFSDesc);
  return USBD_CONTROL_CfgFSDesc;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static uint8_t  *USBD_CONTROL_GetDeviceQualifierDesc(uint16_t *length)
{
  *length = sizeof(USBD_CONTROL_DeviceQualifierDesc);
  return USBD_CONTROL_DeviceQualifierDesc;
}
/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
