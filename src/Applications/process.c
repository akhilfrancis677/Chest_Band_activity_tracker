/*
 * process.c
 *
 *  Created on: 03-May-2020
 *      Author: afrancis
 */

#include "process.h"

int16_t xyz[3] = { 0, 0, 0 };
float xyz_g[3] = { 0, 0, 0 };

float acceleration, tan_acc;
uint8_t first = 0, process = 0,movement = 0;

uint32_t val = 0, timerValue=0, prev_timer_val=0,test_timerValue =0, _test_timerValue = 0, global_data=0, timer_value =0;
extern uint32_t steps = 0;

char data[20];
int modulus, shift, count = 0, total;
extern uint8_t accelrometer_read_status = 0;
extern unsigned char status = 9;

void activity_finder(void)
{


	if (status == 10) {
		BSP_ACCELERO_AccGetXYZ(xyz);
		for (total = 0; total < 3; total++) {
			acceleration = (float) xyz[total] / 1000;
			xyz_g[total] = acceleration * 9.8;
		}
		xyz_g[1] *= xyz_g[1];
		xyz_g[2] *= xyz_g[2];
		xyz_g[1] += xyz_g[2];
		tan_acc = sqrt(xyz_g[1]);
		if (tan_acc < 7.5 && movement == 0) {
			if (tan_acc != 0) {
				HAL_Delay(300);
				movement = 1;
			} else {
				HAL_UART_Transmit(&xConsoleUart,
						(uint8_t *) "STATUS IS STANDING\n", 19, 30000);
			}
		}
		if (movement == 1 && tan_acc > 8.1) {
			process = 1;
			movement = 0;
			steps++;
			timerValue = __HAL_TIM_GET_COUNTER(&timer);
			test_timerValue = timerValue;
			_test_timerValue = timerValue;
			val = steps;
			int diff = timerValue - prev_timer_val;
			if (first != 0 && (diff < 110)) {
				first = 1;
			} else {
				first = 2;
			}
			prev_timer_val = timerValue;
			while (val != 0) {
				modulus = val % 10;
				for (shift = (count - 1); shift >= 0; shift--) {
					data[shift + 1] = data[shift];
				}
				data[0] = modulus + '0';
				val /= 10;
				count++;
			}
			if (first == 2) {
				HAL_UART_Transmit(&xConsoleUart,
						(uint8_t *) "STATUS IS WALKING & STEPS ARE = ", 32,
						30000);

			}
			if (first == 1) {
				HAL_UART_Transmit(&xConsoleUart,
						(uint8_t *) "STATUS IS RUNNING & STEPS ARE = ", 32,
						30000);
			}

			HAL_UART_Transmit(&xConsoleUart, (uint8_t *) &data, count, 30000);
			HAL_UART_Transmit(&xConsoleUart, (uint8_t *) "\n", 1, 30000);
			count = 0;
			accelrometer_read_status = 1;
		} else {

			uint32_t data_timer = __HAL_TIM_GET_COUNTER(&timer);
			uint32_t diff = data_timer - _test_timerValue;

			global_data += diff;
			if (global_data > 30000) {
				HAL_UART_Transmit(&xConsoleUart,
						(uint8_t *) "STATUS IS STANDING\n", 19, 30000);
				global_data = 0;
				if (timerValue <=test_timerValue ||timerValue == 0) {
					_test_timerValue = data_timer;
				}
			}
		}
	}

	else {
			uint32_t prev_timerValue = __HAL_TIM_GET_COUNTER(&timer);
			uint32_t diff = prev_timerValue - test_timerValue;
			global_data += diff;
			if (global_data > 30000) {
				HAL_UART_Transmit(&xConsoleUart,
						(uint8_t *) "STATUS IS STANDING & CONNECT YOUR BLUETOOTH DEVICE\n", 51, 30000);
				global_data = 0;
				if (timerValue != 0) {
					test_timerValue = timerValue;
				} else {
					test_timerValue = prev_timerValue;
				}
			}
		}

}
