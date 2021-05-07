#include "main.h"

uint8_t accelrometer_read_status;

int main(void) {
	HAL_Init();
	__HAL_RCC_PWR_CLK_ENABLE()
	;

	Init_RTC();
	HW_TS_Init(hw_ts_InitMode_Full, &hrtc);

	SystemClock_Config();

	Console_UART_Init();
	timer_init();

	while (1) {
		endless_loop();
	}

}

void endless_loop(void)
{
	PROXI_AdvUpdate();
	TL_BLE_HCI_UserEvtProc();
	if (accelrometer_read_status == 1) {
		PROXISAPP_Measurement();
	}

}
static void Init_RTC(void) {
	__HAL_RCC_LSI_ENABLE(); /**< Enable the LSI clock */

	HAL_PWR_EnableBkUpAccess(); /**< Enable access to the RTC registers */

	HAL_PWR_EnableBkUpAccess();

	__HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSI); /**< Select LSI as RTC Input */

	__HAL_RCC_RTC_ENABLE(); /**< Enable RTC */

	hrtc.Instance = RTC; /**< Define instance */

	HAL_RTCEx_EnableBypassShadow(&hrtc);

	hrtc.Init.AsynchPrediv = CFG_RTC_ASYNCH_PRESCALER;
	hrtc.Init.SynchPrediv = CFG_RTC_SYNCH_PRESCALER;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24; /**< need to be initialized to not corrupt the RTC_CR register */
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH; /**< need to be initialized to not corrupt the RTC_CR register */
	HAL_RTC_Init(&hrtc);

	__HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc)
	; /**< Disable Write Protection */

	LL_RTC_WAKEUP_SetClock(hrtc.Instance, CFG_RTC_WUCKSEL_DIVIDER);

	while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == 0)
		; /**< Wait for LSI to be stable */

	return;
}


void timer_init(void)
{
	__GPIOD_CLK_ENABLE()
		;
		__TIM2_CLK_ENABLE()
		;
		__TIM4_CLK_ENABLE()
		;

		SCH_RegTask(CFG_IdleTask_MeasReq, PROXISAPP_Measurement);
		SCH_RegTask(CFG_IdleTask_HciAsynchEvt, TL_BLE_HCI_UserEvtProc);
		SCH_RegTask(CFG_IdleTask_TlEvt, TL_BLE_R_EvtProc);
		SCH_RegTask(CFG_IdleTask_ConnMgr, PROXI_AdvUpdate);

		PROXI_Init(PROXI_Full);

		timer.Instance = TIM2;
		timer.Init.Prescaler = 40000;
		timer.Init.CounterMode = TIM_COUNTERMODE_UP;
		timer.Init.Period = 4294967295;
		timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		timer.Init.RepetitionCounter = 0;
		HAL_TIM_Base_Init(&timer);
		HAL_TIM_Base_Start(&timer);

		timer_intr.Instance = TIM4;
		timer_intr.Init.Prescaler = 40000;
		timer_intr.Init.CounterMode = TIM_COUNTERMODE_UP;
		timer_intr.Init.Period = 15;
		timer_intr.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		timer_intr.Init.RepetitionCounter = 0;
		HAL_TIM_Base_Init(&timer_intr);
		HAL_TIM_Base_Start_IT(&timer_intr);
		HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM4_IRQn);
		BSP_ACCELERO_Init();
}
void TIM4_IRQHandler(void) {
	activity_finder();
	HAL_TIM_IRQHandler(&timer_intr);
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

#if (HSI_WITH_PLL == 1)
	/**
	 * Set to Range1 has the reset value has been changed in system_stm32yyxx.c to Range2
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	/**
	 *  Enable HSI oscillator and configure the PLL to reach the max system frequency
	 *  (80MHz) when using HSI oscillator as PLL clock source.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV17;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV8;

	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/**
	 *  Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers.
	 *  The SysTick 1 msec interrupt is required for the HAL process (Timeout management); by default
	 *  the configuration is done using the HAL_Init() API, and when the system clock configuration
	 *  is updated the SysTick configuration will be adjusted by the HAL_RCC_ClockConfig() API.
	 */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV16;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV8;

	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);

#elif (HSI_WITHOUT_PLL == 1)
	/**
	 * The device has been already set to Range2 in system_stm32yyxx.c
	 */

	/**
	 *  Enable HSI oscillator and configure the system at 16MHz
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;

	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/**
	 *  Configure the HCLK, PCLK1 and PCLK2 clocks dividers  to get 8Mhz.
	 *  The SysTick 1 msec interrupt is required for the HAL process (Timeout management); by default
	 *  the configuration is done using the HAL_Init() API, and when the system clock configuration
	 *  is updated the SysTick configuration will be adjusted by the HAL_RCC_ClockConfig() API.
	 */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
#elif (MSI_WITHOUT_PLL == 1)
	/**
	 * The device has been already set to Range2 in system_stm32yyxx.c
	 */

	/**
	 *  Enable MSI oscillator and configure the system at 16MHz
	 *  The best power consumption are achieved with 8Mhz on the SPI to access the BlueNRG device
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_8;
	RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;

	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/**
	 *  Configure the HCLK, PCLK1 and PCLK2 clocks dividers  to a maximum of 16Mhz on SPI.
	 */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
#else
#error Missing System Clock Configuration
#endif

	return;
}

/* ******************************************************************************/
/* ******  Exported functions to OVERLOAD __weak defined in /Common modules *****/
/* ******************************************************************************/

/**
 * @brief  This function implement the application action when a EXTI IRQ is raised
 *         It OVERLOADS the __WEAK function defined in stm32l4xx_hal_gpio.h/c
 * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	switch (GPIO_Pin) {
	case BNRG_SPI_EXTI_PIN:
		HW_BNRG_SpiIrqCb();
		break;

	default:
		break;
	}

	return;
}

/**
 * @brief  This function is empty as the SysTick Timer is not used
 *         It OVERLOADS the __WEAK function defined in stm32l4xx_hal.c
 * @param Delay: specifies the delay time length, in milliseconds.
 * @retval None
 */
void HAL_Delay(__IO uint32_t Delay) {
	return;
}

/**
 * @brief  This function OVERLOADEDs the one defined as __WEAK in the LPM (LowPowerManager)
 *         The application should implement dedicated code before getting out from Stop Mode.
 *         This is where the application should reconfigure the clock tree when needed
 * @param  None
 * @retval None
 */
void LPM_ExitStopMode(void) {
#if (HSI_WITH_PLL == 1)
	__HAL_RCC_PLL_ENABLE(); /**< Enable PLL */
	while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET); /**< Wait till PLL is ready */
	LL_RCC_SetSysClkSource(RCC_CFGR_SW_PLL); /**< Select PLL as system clock source */
	while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL); /**< Wait till PLL is used as system clock source */
#endif

	return;
}

/**
 * @brief  This function OVERLOADEDs the one defined as __WEAK in the LPM (LowPowerManager)
 *         It sets the IO configuration to be applied when in standby mode
 * @param  None
 * @retval None
 */
void LPM_EnterOffMode(void) {
	LL_PWR_EnableWakeUpPin(BNRG_SPI_IRQ_WAKEUP_PIN);
	LL_PWR_EnablePUPDCfg();

	return;

}

/**
 * @brief  This function OVERLOADEDs the one defined as __WEAK in the LPM (LowPowerManager)
 *         The application should implement dedicated code before getting out from Off mode
 * @param  None
 * @retval None
 */
void LPM_ExitOffMode(void) {
	LL_PWR_DisablePUPDCfg();
	LL_PWR_DisableWakeUpPin(BNRG_SPI_IRQ_WAKEUP_PIN);
	BNRG_SPI_CLEAR_WAKEUP_FLAG();

	return;
}

/**
 * @brief  This function is empty to avoid starting the SysTick Timer
 *         It OVERLOADS the __WEAK function defined in stm32l4xx_hal.c
 * @param TickPriority: Tick interrupt priority.
 * @retval HAL status
 */
//HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
//{
//  return (HAL_OK);
//}
/**
 * @brief This function OVERLOADEDs the one defined as __WEAK in the SCH (scheduler)
 *        The application should enter low power mode
 * @param  None
 * @retval None
 */
void SCH_Idle(void) {
	return;
}

/**
 * @brief This function OVERLOADEDs the one defined as __WEAK in the TL (transport layer), because
 *        only the application knows which TASKs are currently running and should be paused/resumed.
 *        The applicaiton shall not send a new command when TL the status is set to TL_BLE_HCI_CmdBusy.
 *        At least, TL_BLE_HCI_UserEvtProc shall not be executed while the TL remains busy
 * @param status
 * @retval None
 */
void TL_BLE_HCI_StatusNot(TL_BLE_HCI_CmdStatus_t status) {
	switch (status) {
	case TL_BLE_HCI_CmdBusy:
		SCH_PauseTask(CFG_IdleTask_ConnMgr);
		SCH_PauseTask(CFG_IdleTask_MeasReq);
		SCH_PauseTask(CFG_IdleTask_HciAsynchEvt);

#if (CFG_BLE_HCI_STDBY == 0)
		LPM_SetOffMode(CFG_LPM_HCI_CmdEvt, LPM_OffMode_Dis);
#endif
		break;

	case TL_BLE_HCI_CmdAvailable:
		SCH_ResumeTask(CFG_IdleTask_ConnMgr);
		SCH_ResumeTask(CFG_IdleTask_MeasReq);
		SCH_ResumeTask(CFG_IdleTask_HciAsynchEvt);

#if (CFG_BLE_HCI_STDBY == 0)
		LPM_SetOffMode(CFG_LPM_HCI_CmdEvt, LPM_OffMode_En);
#endif
		break;

	default:
		break;
	}
	return;
}

/**
 * @brief  This function OVERLOADEDs the one defined as __WEAK in the TS (TimeServer)
 *         This API is running in the RTC Wakeup interrupt context.
 *         The application may implement an Operating System to change the context priority where the timer
 *         callback may be handled. This API provides the module ID to identify which module is concerned and to allow
 *         sending the information to the correct task
 *
 * @param  TimerProcessID: The TimerProcessId associated with the timer when it has been created
 * @param  TimerID: The TimerID of the expired timer
 * @param  pTimerCallBack: The Callback associated with the timer when it has been created
 * @retval None
 */

void HW_TS_RTC_Int_AppNot(uint32_t eTimerProcessID, uint8_t ubTimerID,
		HW_TS_pTimerCb_t pfTimerCallBack) {
	switch (eTimerProcessID) {
	case CFG_TimProcID_isr:
		/**
		 * This is expected to be called in the Timer Interrupt Handler to limit latency
		 */
		pfTimerCallBack();
		break;

	default:
		pfTimerCallBack();
		break;
	}
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
