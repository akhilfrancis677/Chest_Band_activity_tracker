/*
 * process.h
 *
 *  Created on: 03-May-2020
 *      Author: afrancis
 */

#ifndef APPLICATIONS_PROCESS_H_
#define APPLICATIONS_PROCESS_H_

#include "stdint.h"
#include "stm32l4xx_hal.h"
#include "math.h"
#include "stm32l475e_iot01_accelero.h"


TIM_HandleTypeDef timer;
UART_HandleTypeDef xConsoleUart;

void activity_finder(void);


#endif /* APPLICATIONS_PROCESS_H_ */
