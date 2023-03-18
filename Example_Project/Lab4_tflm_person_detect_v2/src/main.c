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

#include "synopsys_sdk_camera_drv.h"

DEV_UART * uart0_ptr;
char uart_buf[uart_buf_size] = {0};

extern uint32_t g_wdma2_baseaddr;

int8_t test_img [kNumRows * kNumCols] = {0};
uint8_t output_img [kNumRows * kNumCols] = {0};
uint8_t output_height = kNumRows;
uint8_t output_width = kNumCols;

int main(void)
{
    //UART 0 is already initialized with 115200bps
	printf("This is Lab4_TFLM_Person_Detect\r\n");

    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);

    synopsys_camera_init();

    tflitemicro_algo_init();

    sprintf(uart_buf, "Start While Loop\r\n");
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(1000);
    while(1)
    {
        sprintf(uart_buf, "While Loop\r\n");
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);

        sprintf(uart_buf, "Start to capture\r\n"); 
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);

        synopsys_camera_start_capture();
        board_delay_ms(100);

        uint8_t * img_ptr;
        uint32_t img_width = 640;
        uint32_t img_height = 480;
        img_ptr = (uint8_t *) g_wdma2_baseaddr;

        synopsys_camera_down_scaling(img_ptr, img_width, img_height, &output_img[0], output_width, output_height);
     
        for(uint32_t pixel_index = 0; pixel_index < kImageSize; pixel_index ++)
            test_img[pixel_index] = output_img[pixel_index] - 128;

        int32_t test_result = tflitemicro_algo_run(&test_img[0]);
        sprintf(uart_buf, "Person_Score: %4d | ", test_result);
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);

        if(test_result > 0)
            sprintf(uart_buf, "Person detect \r\n");
        else 
            sprintf(uart_buf, "Person not detect \r\n");

        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);
        
        board_delay_ms(1000);
    }

	return 0;
}

