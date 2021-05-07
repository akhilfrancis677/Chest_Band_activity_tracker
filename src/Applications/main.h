/*
 * main.h
 *
 *  Created on: 03-May-2020
 *      Author: afrancis
 */

#ifndef APPLICATIONS_MAIN_H_
#define APPLICATIONS_MAIN_H_

#include "common.h"
#include "hw.h"
#include "lpm.h"
#include "proxi.h"
#include "scheduler.h"

#include "tl_ble_reassembly.h"
#include "tl_ble_hci.h"
#include "proxis_app.h"

#include "rtos_task.h"
#include "math.h"
#include "process.h"

static RTC_HandleTypeDef hrtc;
static void Init_RTC(void);
void SystemClock_Config(void);
void timer_init(void);
void endless_loop(void);

//
//int16_t xyz[3] = { 0, 0, 0 };
//float xyz_g[3] = { 0, 0, 0 };
//
//float acceleration, tan_acc;
//uint8_t first = 0, process = 0,movement = 0;
//
//uint32_t val = 0, timerValue=0, prev_timer_val=0,test_timerValue =0, _test_timerValue = 0, global_data=0, timer_value =0;
//extern uint32_t steps = 0;
//TIM_HandleTypeDef timer;
TIM_HandleTypeDef timer_intr;
//char data[20];
//int modulus, shift, count = 0, total;
//
//uint8_t accelrometer_read_status = 0;
UART_HandleTypeDef xConsoleUart;


#endif /* APPLICATIONS_MAIN_H_ */
