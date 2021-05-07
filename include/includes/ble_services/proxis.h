/*
 * proxis.h
 *
 *  Created on: 03-May-2020
 *      Author: afrancis
 */

#ifndef PROXIS_H_
#define PROXIS_H_

typedef enum
  {
    PROXI_SENSOR_CONTACT_PRESENT = 2,

  } PROXIS_HrmFlags_t;


  typedef enum
  {
    PROXIS_RESET_ENERGY_EXPENDED_EVT,
	PROXIS_NOTIFICATION_ENABLED,
	PROXIS_NOTIFICATION_DISABLED,
  } PROXIS_NotCode_t;

  typedef struct{
    uint16_t    MeasurementValue;
#if (BLE_CFG_PROXIS_ENERGY_EXPENDED_INFO_FLAG == 1)
    uint16_t    EnergyExpended;
#endif
#if (BLE_CFG_PROXIS_ENERGY_RR_INTERVAL_FLAG != 0)
    uint16_t    aRRIntervalValues[BLE_CFG_PROXIS_ENERGY_RR_INTERVAL_FLAG + BLE_CFG_PROXIS_ENERGY_EXPENDED_INFO_FLAG];
    uint8_t     NbreOfValidRRIntervalValues;
#endif
    uint8_t     Flags;
  }PROXIS_MeasVal_t;

  void PROXIS_Init(void);
  tBleStatus PROXIS_UpdateChar(uint16_t uuid, uint8_t *p_payload);
  void PROXIS_Notification(PROXIS_NotCode_t notification);

#endif /* PROXIS_H_ */
