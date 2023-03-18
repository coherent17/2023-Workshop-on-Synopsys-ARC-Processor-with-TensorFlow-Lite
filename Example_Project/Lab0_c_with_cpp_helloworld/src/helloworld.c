/*
 * main.c
 *
 *  Created on: 2021�~5��28��
 *      Author: williet
 */
#include "main.h"
#include "helloworld.h"

void helloworld (void)
{
	sprintf(uart_buf, "Helloworld_1\r\n");
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(100);

	sprintf(uart_buf, "Helloworld_2\r\n");
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(100);

	sprintf(uart_buf, "Helloworld_3\r\n");
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(100);

}



