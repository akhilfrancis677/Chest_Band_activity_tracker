#include "common.h"
#include "hw.h"

#include "ble_lib.h"
#include "blesvc.h"

#include "proxi.h"
#include "dis_app.h"
#include "proxis_app.h"

#include "scheduler.h"
#include "tl_types.h"
#include "tl_ble_hci.h"
#include "lpm.h"
#include "tl_ble_reassembly.h"

typedef enum {
	PROXI_IDLE,
	PROXI_FAST_ADV,
	PROXI_LP_ADV,
	PROXI_LP_CONNECTING,
	PROXI_CONNECTED_SERVER,
	PROXI_CONNECTED_CLIENT
} PROXI_ConnStatus_t;

/**
 * security parameters structure
 */
typedef struct _tSecurityParams {
	/**
	 * IO capability of the device
	 */
	uint8_t ioCapability;

	/**
	 * Authentication requirement of the device
	 * Man In the Middle protection required?
	 */
	uint8_t mitm_mode;

	/**
	 * bonding mode of the device
	 */
	uint8_t bonding_mode;

	/**
	 * Flag to tell whether OOB data has
	 * to be used during the pairing process
	 */
	uint8_t OOB_Data_Present;

	/**
	 * OOB data to be used in the pairing process if
	 * OOB_Data_Present is set to TRUE
	 */
	uint8_t OOB_Data[16];

	/**
	 * this variable indicates whether to use a fixed pin
	 * during the pairing process or a passkey has to be
	 * requested to the application during the pairing process
	 * 0 implies use fixed pin and 1 implies request for passkey
	 */
	uint8_t Use_Fixed_Pin;

	/**
	 * minimum encryption key size requirement
	 */
	uint8_t encryptionKeySizeMin;

	/**
	 * maximum encryption key size requirement
	 */
	uint8_t encryptionKeySizeMax;

	/**
	 * fixed pin to be used in the pairing process if
	 * Use_Fixed_Pin is set to 1
	 */
	uint32_t Fixed_Pin;

	/**
	 * this flag indicates whether the host has to initiate
	 * the security, wait for pairing or does not have any security
	 * requirements.\n
	 * 0x00 : no security required
	 * 0x01 : host should initiate security by sending the slave security
	 *        request command
	 * 0x02 : host need not send the clave security request but it
	 * has to wait for paiirng to complete before doing any other
	 * processing
	 */
	uint8_t initiateSecurity;
} tSecurityParams;

/**
 * global context
 * contains the variables common to all 
 * services
 */
typedef struct _tBLEProfileGlobalContext {

	/**
	 * security requirements of the host
	 */
	tSecurityParams bleSecurityParam;

	/**
	 * gap service handle
	 */
	uint16_t gapServiceHandle;

	/**
	 * device name characteristic handle
	 */
	uint16_t devNameCharHandle;

	/**
	 * appearance characteristic handle
	 */
	uint16_t appearanceCharHandle;

	/**
	 * connection handle of the current active connection
	 * When not in connection, the handle is set to 0xFFFF
	 */
	uint16_t connectionHandle[CFG_MAX_CONNECTION];

	/**
	 * length of the UUID list to be used while advertising
	 */
	uint8_t advtServUUIDlen;

	/**
	 * the UUID list to be used while advertising
	 */
	uint8_t advtServUUID[100];

} BleGlobalContext_t;

typedef struct {
	BleGlobalContext_t BleApplicationContext_legacy;
	PROXI_ConnStatus_t Device_Connection_Status[CFG_MAX_CONNECTION];
	uint8_t Connection_mgr_timer_Id;
} BleApplicationContext_t;

/* Private defines -----------------------------------------------------------*/
#define FAST_ADV_TIMEOUT            (30*1000*1000/CFG_TS_TICK_VAL) /**< 30s */

#define POOL_SIZE (CFG_TLBLE_EVT_QUEUE_LENGTH * ( sizeof(TL_PacketHeader_t) + TL_BLE_EVENT_FRAME_SIZE ))

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
 * START of Section BLE_APP_CONTEXT
 */
static BleApplicationContext_t BleApplicationContext;
static uint16_t AdvIntervalMin, AdvIntervalMax;

/**
 * END of Section BLE_APP_CONTEXT
 */

/*******************************************************************************
 * START of Section BLE_STDBY_MEM
 */
static TL_CmdPacket_t CmdBuffer;

/*******************************************************************************
 * END of Section BLE_STDBY_MEM
 */

/*******************************************************************************
 * START of Section BLE_SHARED_NORET_MEM
 */
static uint8_t EvtPool[POOL_SIZE];

/*******************************************************************************
 * END of Section BLE_SHARED_NORET_MEM
 */

uint8_t Test_Status = 0x00;

//static const char local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'H','R','_','L','4','7','5','_','I','o','T'};

static const char local_name[] = { AD_TYPE_COMPLETE_LOCAL_NAME, 'C', 'H', 'E',
		'S', 'T', '_', 'B', 'A', 'N', 'D'};

/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void Add_Advertisment_Service_UUID(uint16_t servUUID);
static void Adv_Request(PROXI_ConnStatus_t New_Status);
static void ConnMgr(void);

/* Functions Definition ------------------------------------------------------*/
void PROXI_Init(PROXI_InitMode_t InitMode)  //PROXI_Init
{
	uint8_t index;

	if (InitMode != PROXI_Limited) {
		TL_BLE_HCI_Init(TL_BLE_HCI_InitFull, &CmdBuffer, EvtPool, POOL_SIZE);

		SVCCTL_Init();

		for (index = 0; index < CFG_MAX_CONNECTION; index++) {
			BleApplicationContext.Device_Connection_Status[index] = PROXI_IDLE;
			BleApplicationContext.BleApplicationContext_legacy.connectionHandle[index] =
					0xFFFF;
		}

		aci_hal_set_tx_power_level(1, 4);

		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.ioCapability =
				CFG_PROXI_IO_CAPABILITY;
		aci_gap_set_io_capability(
				BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.ioCapability);

		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.mitm_mode =
				CFG_PROXI_MITM_PROTECTION;
		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.OOB_Data_Present =
				0;
		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMin =
				8;
		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMax =
				16;
		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.Use_Fixed_Pin =
				0;
		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.Fixed_Pin =
				111111;
		BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode =
				1;
		for (index = 0; index < 16; index++) {
			BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.OOB_Data[index] =
					(uint8_t) index;
		}

		aci_gap_set_auth_requirement(
				BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.mitm_mode,
				BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.OOB_Data_Present,
				BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.OOB_Data,
				BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMin,
				BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.encryptionKeySizeMax,
				BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.Use_Fixed_Pin,
				BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.Fixed_Pin,
				BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode);

		if (BleApplicationContext.BleApplicationContext_legacy.bleSecurityParam.bonding_mode) {
			aci_gap_configure_whitelist();
		}

//    DISAPP_Init();

		PROXISAPP_Init();

		HW_TS_Create(CFG_TimProcID_isr,
				&(BleApplicationContext.Connection_mgr_timer_Id),
				hw_ts_SingleShot, ConnMgr);

		BleApplicationContext.BleApplicationContext_legacy.advtServUUID[0] =
				AD_TYPE_16_BIT_SERV_UUID;
		BleApplicationContext.BleApplicationContext_legacy.advtServUUIDlen = 1;

		Add_Advertisment_Service_UUID(DISTANCE_RATE_SERVICE_UUID);

		AdvIntervalMin = CFG_PROXI_FAST_CONN_ADV_INTERVAL_MIN;
		AdvIntervalMax = CFG_PROXI_FAST_CONN_ADV_INTERVAL_MAX;

		Adv_Request(PROXI_FAST_ADV);
	} else {

		TL_BLE_HCI_Init(TL_BLE_HCI_InitLimited, &CmdBuffer, EvtPool, POOL_SIZE);
	}

	return;
}

void PROXI_AdvUpdate(void) {

	Adv_Request(PROXI_LP_ADV);  //PROXI_LP_ADV

	return;
}

void Adv_Request(PROXI_ConnStatus_t New_Status) {
	uint16_t Min_Inter, Max_Inter;


	Min_Inter = AdvIntervalMin;
	Max_Inter = AdvIntervalMax;

	aci_gap_set_discoverable(ADV_IND, Min_Inter, Max_Inter,
	PUBLIC_ADDR,
	NO_WHITE_LIST_USE, /* use white list */
	sizeof(local_name), local_name,
			BleApplicationContext.BleApplicationContext_legacy.advtServUUIDlen,
			BleApplicationContext.BleApplicationContext_legacy.advtServUUID, 0,
			0);

	return;
}

void SVCCTL_App_Notification(void *pckt) {
	hci_event_pckt *event_pckt;
	evt_le_meta_event *meta_evt;
	uint8_t index;

	event_pckt = (hci_event_pckt*) ((hci_uart_pckt *) pckt)->data;

	switch (event_pckt->evt) {
	case EVT_DISCONN_COMPLETE: {
		evt_disconn_complete * disconnection_complete_event;

		disconnection_complete_event =
				(evt_disconn_complete *) event_pckt->data;

		/* Find index of the handle deconnected */
		index = 0;
		while ((index < CFG_MAX_CONNECTION)
				&& (BleApplicationContext.BleApplicationContext_legacy.connectionHandle[index]
						!= disconnection_complete_event->handle)) {
			index++;
		}

		if (index < CFG_MAX_CONNECTION) {

			BleApplicationContext.Device_Connection_Status[index] = PROXI_IDLE;
			BleApplicationContext.BleApplicationContext_legacy.connectionHandle[index] =
					0xFFFF;
		} else

			/* restart advertising */
			Adv_Request(PROXI_FAST_ADV);
	}
		break; /* EVT_DISCONN_COMPLETE */

	case EVT_LE_META_EVENT: {
		meta_evt = (evt_le_meta_event*) event_pckt->data;

		switch (meta_evt->subevent) {
		case EVT_LE_CONN_COMPLETE: {
			evt_le_connection_complete * connection_complete_event;

			/**
			 * The connection is done, there is no need anymore to schedule the LP ADV
			 */
			connection_complete_event =
					(evt_le_connection_complete *) meta_evt->data;
			HW_TS_Stop(BleApplicationContext.Connection_mgr_timer_Id);

			/* Find index of a connection not in PROXI_IDLE, PROXI_CONNECTED_SERVER or PROXI_CONNECTED_CLIENT state */
			index = 0;
			while ((index < CFG_MAX_CONNECTION)
					&& ((BleApplicationContext.Device_Connection_Status[index]
							== PROXI_IDLE)
							|| (BleApplicationContext.Device_Connection_Status[index]
									== PROXI_CONNECTED_SERVER)
							|| (BleApplicationContext.Device_Connection_Status[index]
									== PROXI_CONNECTED_CLIENT))) {
				index++;
			}

			if (index < CFG_MAX_CONNECTION) {
				if (BleApplicationContext.Device_Connection_Status[index]
						== PROXI_LP_CONNECTING)
					/* Connection as client */
					BleApplicationContext.Device_Connection_Status[index] =
							PROXI_CONNECTED_CLIENT;
				else
					/* Connection as server */
					BleApplicationContext.Device_Connection_Status[index] =
							PROXI_CONNECTED_SERVER;
				BleApplicationContext.BleApplicationContext_legacy.connectionHandle[index] =
						connection_complete_event->handle;
			} else {
			}
		}
			break; /* HCI_EVT_LE_CONN_COMPLETE */

		default:
			break;
		}
	}
		break; /* HCI_EVT_LE_META_EVENT */

	default:
		break;
	}

	return;
}

static void Add_Advertisment_Service_UUID(uint16_t servUUID) {
	BleApplicationContext.BleApplicationContext_legacy.advtServUUID[BleApplicationContext.BleApplicationContext_legacy.advtServUUIDlen] =
			(uint8_t) (servUUID & 0xFF);
	BleApplicationContext.BleApplicationContext_legacy.advtServUUIDlen++;
	BleApplicationContext.BleApplicationContext_legacy.advtServUUID[BleApplicationContext.BleApplicationContext_legacy.advtServUUIDlen] =
			(uint8_t) (servUUID >> 8) & 0xFF;
	BleApplicationContext.BleApplicationContext_legacy.advtServUUIDlen++;

	return;
}

static void ConnMgr(void) {
	/**
	 * The code shall be executed in the background as an aci command may be sent
	 * The background is the only place where the application can make sure a new aci command
	 * is not sent if there is a pending one
	 */
	SCH_SetTask(CFG_IdleTask_ConnMgr);

	return;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
