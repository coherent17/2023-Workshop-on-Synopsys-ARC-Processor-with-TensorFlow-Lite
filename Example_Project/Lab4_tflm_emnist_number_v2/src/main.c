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

extern uint32_t g_wdma2_baseaddr;

int8_t test_img [kNumRows * kNumCols] = {0};
uint8_t output_img [kNumRows * kNumCols] = {0};
uint8_t output_height = kNumRows;
uint8_t output_width = kNumCols;

DEV_UART * uart1_ptr;
char str_buf[100];

#define TEST_UPPER 25
uint8_t test_cnt = 0;
uint8_t test_correct = 0;
#include "test_samples.h"

int main(void)
{

    printf("\n\nversion 1\n\n");

    uart1_ptr = hx_drv_uart_get_dev(USE_SS_UART_1);
    uart1_ptr->uart_open(UART_BAUDRATE_115200);

    synopsys_camera_init();

    tflitemicro_algo_init();

    sprintf(str_buf, "Start While Loop\r\n");
    uart1_ptr->uart_write(str_buf, strlen(str_buf));
    board_delay_ms(1000);
    while(test_cnt < TEST_UPPER)
    {
        printf("Loop_Start\n");  
        sprintf(str_buf, "Loop_Start\r\n");
        uart1_ptr->uart_write(str_buf, strlen(str_buf));
        board_delay_ms(1000);

        printf("Start_to_Capture\n");   
        sprintf(str_buf, "Start_to_Capture\r\n"); 
        uart1_ptr->uart_write(str_buf, strlen(str_buf));

        synopsys_camera_start_capture();
        board_delay_ms(100);

        uint8_t * img_ptr;
        uint32_t img_width = 640;
        uint32_t img_height = 480;
        img_ptr = (uint8_t *) g_wdma2_baseaddr;

        synopsys_camera_down_scaling(img_ptr, img_width, img_height, &output_img[0], output_width, output_height);
        /*
        sprintf(str_buf, "Send Image Start\r\n"); 
        uart1_ptr->uart_write(str_buf, strlen(str_buf));
            board_delay_ms(5);  
        for(uint32_t heigth_cnt = 0; heigth_cnt < output_height; heigth_cnt ++)
        {
            for(uint32_t width_cnt = 0; width_cnt < output_width; width_cnt ++)
            {                        
                if(width_cnt != (output_width - 1))
                    sprintf(str_buf, "%3d, ", output_img[(heigth_cnt * output_width) + width_cnt]);
                else
                    sprintf(str_buf, "%3d\r\n", output_img[(heigth_cnt * output_width) + width_cnt]);

                printf(str_buf);   
                uart1_ptr->uart_write(str_buf, strlen(str_buf));
                    board_delay_ms(1);  
            }
        }
        sprintf(str_buf, "Send Image End\r\n"); 
        uart1_ptr->uart_write(str_buf, strlen(str_buf));
            board_delay_ms(5);  
        */

        for(uint32_t pixel_index = 0; pixel_index < kImageSize; pixel_index ++)
            test_img[pixel_index] = output_img[pixel_index] - 128;

        int32_t test_result = tflitemicro_algo_run(&test_img[0]);
        printf("Answer: %2d\r\n\n", test_result);
        sprintf(str_buf, "Answer: %2d\r\n\n", test_result);
        uart1_ptr->uart_write(str_buf, strlen(str_buf));
        board_delay_ms(1000);
        
        /*
        printf("test_cnt: %2d\r\n", test_cnt + 1);
        sprintf(str_buf, "test_cnt: %2d\r\n", test_cnt + 1);
        uart1_ptr->uart_write(str_buf, strlen(str_buf));
        board_delay_ms(10);

        for(uint32_t pixel_index = 0; pixel_index < kImageSize; pixel_index ++)
            test_img[pixel_index] = test_samples[test_cnt].image[pixel_index] - 128;
            //test_img[pixel_index] = (test_samples[test_cnt].image[pixel_index] <= 210) ? -128 : 127;

        int32_t test_result = tflitemicro_algo_run(&test_img[0]);

        if(test_samples[test_cnt].label == test_result)
            test_correct ++;
            
        printf("Answer: %2d | Predict: %2d\r\n\n", test_samples[test_cnt].label, test_result);
        sprintf(str_buf, "Answer: %2d | Predict: %2d\r\n\n", test_samples[test_cnt].label, test_result);
        uart1_ptr->uart_write(str_buf, strlen(str_buf));
        board_delay_ms(100);

        test_cnt ++;
        */
    }

    printf("Accuary: %2d / %2d\r\n", test_correct, test_cnt);
    sprintf(str_buf, "Accuary: %2d / %2d\r\n", test_correct, test_cnt);
    uart1_ptr->uart_write(str_buf, strlen(str_buf));
    board_delay_ms(100);

	return 0;
}

