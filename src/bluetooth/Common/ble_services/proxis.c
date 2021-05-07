#include "config.h"
#include "svc_private.h"
#include "ble_lib.h"
#include "svc_ctl.h"
#include "proxis.h"

typedef struct
{
  uint16_t  DistanceRateSvcHdle;        /**< Service handle */
  uint16_t  DistanceRatemeasurementCharHdle;  /**< Characteristic handle */
}PROXIS_Context_t;


#define PROXIS_CNTL_POINT_RESET_ENERGY_EXPENDED      (0x01)
#define PROXIS_CNTL_POINT_VALUE_IS_SUPPORTED         (0x00)
#define PROXIS_CNTL_POINT_VALUE_NOT_SUPPORTED        (0x80)

#define PROXIS_MAX_NBR_RR_INTERVAL_VALUES            9


static PROXIS_Context_t PROXIS_Context;

static tBleStatus Update_Char_Measurement(PROXIS_MeasVal_t *pMeasurement );
static SVCCTL_EvtAckStatus_t HearRate_Event_Handler(void *pckt);


static SVCCTL_EvtAckStatus_t HearRate_Event_Handler(void *Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *event_pckt;
  evt_blue_aci *blue_evt;
  evt_gatt_attr_modified    * attribute_modified;

  return_value = SVCCTL_EvtNotAck;
  event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)Event)->data);

  switch(event_pckt->evt)
  {
    case EVT_VENDOR:
    {
      blue_evt = (evt_blue_aci*)event_pckt->data;
      switch(blue_evt->ecode)
      {

        case EVT_BLUE_GATT_ATTRIBUTE_MODIFIED:
        {
          attribute_modified = (evt_gatt_attr_modified*)blue_evt->data;
          if(attribute_modified->attr_handle == (PROXIS_Context.DistanceRatemeasurementCharHdle + 2))
          {
            if(attribute_modified->att_data[0] & COMSVC_Notification)
            {
            	PROXIS_Notification(PROXIS_NOTIFICATION_ENABLED);
            }
            else
            {
            	PROXIS_Notification(PROXIS_NOTIFICATION_DISABLED);
            }
          }
        }
        break;

        default:
          break;
      }
    }
    break;

    default:
      break;
  }

  return(return_value);
}

static tBleStatus Update_Char_Measurement (PROXIS_MeasVal_t *pMeasurement )
{
  tBleStatus return_value;

  uint8_t adistancem_value[2];

  uint8_t distancem_char_length;

  adistancem_value[0] = (uint8_t)pMeasurement->Flags;
  distancem_char_length = 1;

  adistancem_value[distancem_char_length] = (uint8_t)(pMeasurement->MeasurementValue & 0xFF);
  distancem_char_length++;

  return_value = aci_gatt_update_char_value(PROXIS_Context.DistanceRateSvcHdle,
                                            PROXIS_Context.DistanceRatemeasurementCharHdle,
                                            0,
                                            distancem_char_length,
                                            (const uint8_t *) &adistancem_value[0]);

  return return_value;
}


void PROXIS_Init(void)
{
  uint16_t uuid;

  SVCCTL_RegisterSvcHandler(HearRate_Event_Handler);

  uuid = DISTANCE_RATE_SERVICE_UUID;
  aci_gatt_add_serv(UUID_TYPE_16,
                                   (const uint8_t *) &uuid,
                                   PRIMARY_SERVICE,
                                   4
                                   +(2*BLE_CFG_PROXIS_NUMBER_OF_CUSTOM_CHAR),
                                   &(PROXIS_Context.DistanceRateSvcHdle));

  uuid = DISTANCE_RATE_MEASURMENT_UUID;
  aci_gatt_add_char(PROXIS_Context.DistanceRateSvcHdle,
                                   UUID_TYPE_16,
                                   (const uint8_t *) &uuid ,
                                   +1
                                   +2,
                                   CHAR_PROP_NOTIFY,
                                   ATTR_PERMISSION_NONE,
                                   GATT_DONT_NOTIFY_EVENTS,
                                   10,
                                   1,
                                   &(PROXIS_Context.DistanceRatemeasurementCharHdle));


  return;
}


tBleStatus PROXIS_UpdateChar(uint16_t UUID, uint8_t *pPayload)
{
  tBleStatus return_value;
  switch(UUID)
  {
    case DISTANCE_RATE_MEASURMENT_UUID:
      return_value = Update_Char_Measurement((PROXIS_MeasVal_t*)pPayload);
      break;

    default:
      return_value = BLE_STATUS_ERROR;
      break;
  }

  return return_value;
}
