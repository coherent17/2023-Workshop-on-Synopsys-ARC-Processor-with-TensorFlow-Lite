/**************************************************************************************************
    (C) COPYRIGHT, Himax Technologies, Inc. ALL RIGHTS RESERVED
    ------------------------------------------------------------------------
    File        : main.c
    Project     : WEI
    DATE        : 2018/10/01
    AUTHOR      : 902452
    BRIFE       : main function
    HISTORY     : Initial version - 2018/10/01 created by Will
    			: V1.0			  - 2018/11/13 support CLI
**************************************************************************************************/
#include "main.h"

#include "tflitemicro_algo.h"
#include "model_settings.h"
#include "test_samples.h"

DEV_UART * uart0_ptr;
char uart_buf[uart_buf_size] = {0};

int8_t input_buf[kImageSize] = {0};

int main(void)
{    
    //UART 0 is already initialized with 115200bps
	printf("This is Lab5_TFLM_EMNIST_Letter\r\n");

    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);

    int test_i = 0;
    int test_j = 0;
    int test_correct = 0;
    int test_result = 0;
    
    sprintf(uart_buf, "TFLM EMNIST Start\r\n");    
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(10);

    tflitemicro_algo_init();

    for(test_i = 0; test_i < kNumSamples; test_i ++)
    {
        for(test_j = 0; test_j < kImageSize; test_j ++)
            input_buf[test_j] = (test_samples[test_i].image[test_j] <= 210) ? -128 : 127;

        sprintf(uart_buf, "Test Number: %2d\r\n", test_i);
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);

        test_result = tflitemicro_algo_run(&input_buf[0]);

        sprintf(uart_buf, "Ans_Num: %2d, TFLM_Num: %2d\r\n", test_samples[test_i].label, test_result);
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);

        sprintf(uart_buf, "Ans_Char: %s, TFLM_Char: %s\r\n\n", kCategoryLabels[test_samples[test_i].label], kCategoryLabels[test_result]);
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);
        
        if(test_samples[test_i].label == test_result)
            test_correct ++;

        // board_delay_ms(300); 
    }

    sprintf(uart_buf, "\nCorrect Rate: %2d / %2d\r\n", test_correct, test_i);
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(10);
    while(1);


	return 0;
}

