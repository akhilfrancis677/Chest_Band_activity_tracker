#include "common.h"
#include "hw.h"
#include "proxis_app.h"
#include "ble_lib.h"
#include "blesvc.h"
#include "scheduler.h"

uint32_t steps;

typedef struct
{
  PROXIS_MeasVal_t MeasurementvalueChar;
  uint8_t ResetEnergyExpended;
  uint8_t TimerMeasurement_Id;

} PROXISAPP_Context_t;


#define PROXISAPP_MEASUREMENT_INTERVAL   (1000000/CFG_TS_TICK_VAL)  /**< 1s */

static PROXISAPP_Context_t PROXISAPP_Context;  // PROXISAPP_Context

static void ProxiMeas( void ); //ProxiMeas

static void ProxiMeas( void )
{
  /**
   * The code shall be executed in the background as aci command may be sent
   * The background is the only place where the application can make sure a new aci command
   * is not sent if there is a pending one
   */
  SCH_SetTask(CFG_IdleTask_MeasReq);

  return;
}



void PROXIS_Notification(PROXIS_NotCode_t Notification)
{
  switch(Notification)
  {

    case PROXIS_NOTIFICATION_ENABLED:

      HW_TS_Stop(PROXISAPP_Context.TimerMeasurement_Id);
      HW_TS_Start(PROXISAPP_Context.TimerMeasurement_Id, PROXISAPP_MEASUREMENT_INTERVAL);
      break;

    case PROXIS_NOTIFICATION_DISABLED:
      HW_TS_Stop(PROXISAPP_Context.TimerMeasurement_Id);
      break;

   default:
      break;
  }

  return;
}

void PROXISAPP_Init(void)
{
  PROXISAPP_Context.MeasurementvalueChar.Flags = (   PROXI_SENSOR_CONTACT_PRESENT );

  HW_TS_Create(CFG_TimProcID_isr, &(PROXISAPP_Context.TimerMeasurement_Id), hw_ts_Repeated, ProxiMeas);

  return;
}

void PROXISAPP_Measurement(void)
{
  uint32_t measurement;
  measurement =  steps;

  PROXISAPP_Context.MeasurementvalueChar.MeasurementValue = measurement;

  PROXIS_UpdateChar(DISTANCE_RATE_MEASURMENT_UUID, (uint8_t *)&PROXISAPP_Context.MeasurementvalueChar);

  return;
}
