/*
 * print_tasks.c
 *
 *  Created on: 26-Apr-2020
 *      Author: afrancis
 */

#include "print_tasks.h"
#include "stm32l475e_iot01.h"


UART_HandleTypeDef xConsoleUart;

void print_data_task_1(void *pv)
{
	while (1)
	{
    HAL_UART_Transmit( &xConsoleUart, ( uint8_t * )pv, 18, 30000 );
	}
}

void print_data_task_2(void *pv)
{
	while (1)
	{
    HAL_UART_Transmit( &xConsoleUart, ( uint8_t * )pv, 18, 30000 );
	}
}


void Console_UART_Init( void )
{
    xConsoleUart.Instance = USART1;
    xConsoleUart.Init.BaudRate = 115200;
    xConsoleUart.Init.WordLength = UART_WORDLENGTH_8B;
    xConsoleUart.Init.StopBits = UART_STOPBITS_1;
    xConsoleUart.Init.Parity = UART_PARITY_NONE;
    xConsoleUart.Init.Mode = UART_MODE_TX_RX;
    xConsoleUart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    xConsoleUart.Init.OverSampling = UART_OVERSAMPLING_16;
    xConsoleUart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    xConsoleUart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    BSP_COM_Init( COM1, &xConsoleUart );
}



