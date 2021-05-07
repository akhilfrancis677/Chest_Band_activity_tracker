/**
 ******************************************************************************
 * @file    dis.c
 * @author  MCD Application Team
 * @brief   Device Information Service
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license SLA0044,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        http://www.st.com/SLA0044
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>   

#include "config.h"   /* provided by the applic. see config/ble_config_templete.h */
//#include "debug.h"    /* provided by the applic. see config/ble_debug_templete.h */

#include "svc_private.h"
#include "ble_lib.h"
#include "svc_ctl.h"
#include "dis.h"


typedef struct
{
  uint16_t  DeviceInformationSvcHdle;       /**< Service handle */
  uint16_t  ManufacturerNameStringCharHdle; /**< Characteristic handle */
}DIS_Context_t;


static DIS_Context_t DIS_Context;

void DIS_Init(uint16_t *options)
{
  uint16_t uuid;

  memset ( &DIS_Context, 0, sizeof(DIS_Context_t) );

  uuid = DEVICE_INFORMATION_SERVICE_UUID;
  aci_gatt_add_serv(UUID_TYPE_16,
                                   (const uint8_t *) &uuid,
                                   PRIMARY_SERVICE,
                                   1

                                   +2
                                   +(2*BLE_CFG_DIS_NUMBER_OF_CUSTOM_CHAR),   /**< Custom Characteristic */
                                   &(DIS_Context.DeviceInformationSvcHdle));

  uuid = MANUFACTURER_NAME_UUID;
  aci_gatt_add_char(DIS_Context.DeviceInformationSvcHdle,
                                   UUID_TYPE_16,
                                   (const uint8_t *) &uuid ,
                                   BLE_CFG_DIS_MANUFACTURER_NAME_STRING_LEN_MAX,
                                   CHAR_PROP_READ,
                                   ATTR_PERMISSION_NONE,
                                   GATT_DONT_NOTIFY_EVENTS, /* gattEvtMask */
                                   10, /* encryKeySize */
                                   CHAR_VALUE_LEN_VARIABLE, /* isVariable */
                                   &(DIS_Context.ManufacturerNameStringCharHdle));


  return;
}


tBleStatus DIS_UpdateChar(uint16_t UUID, DIS_Data_t *pPData)
{
  tBleStatus return_value;

  switch(UUID)
  {
    case MANUFACTURER_NAME_UUID:
      return_value = aci_gatt_update_char_value(DIS_Context.DeviceInformationSvcHdle,
                                                DIS_Context.ManufacturerNameStringCharHdle,
                                                0,
                                                pPData->Length,
                                                (const uint8_t *)pPData->pPayload);
      break;
    default:
      return_value = BLE_STATUS_ERROR;
      break;
  }
  return return_value;
}
