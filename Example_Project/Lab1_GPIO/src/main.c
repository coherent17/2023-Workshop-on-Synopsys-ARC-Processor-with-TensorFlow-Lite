#include "main.h"

#include "hx_drv_iomux.h"

DEV_UART * uart0_ptr;
char uart_buf[uart_buf_size] = {0};

char gpio_value;
uint32_t timing_sec_cnt;

int main(void)
{
    //UART 0 is already initialized with 115200bps
    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);

    sprintf(uart_buf, "This is Lab1_GPIO\r\n");
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(1000);
        
    hx_drv_iomux_init();
    hx_drv_iomux_set_pmux(IOMUX_PGPIO1, 2); //CPU Board IR Senosr Input
/*
    hx_drv_iomux_set_pmux(IOMUX_PGPIO4, 3); //CPU Board LED Output
    hx_drv_iomux_set_pmux(IOMUX_PGPIO5, 3); //CPU Board LED Output
    hx_drv_iomux_set_pmux(IOMUX_PGPIO11, 3); //CPU Board LED Output
*/
    hx_drv_iomux_set_pmux(IOMUX_PGPIO8, 3); //Extension Board Red LED Output
    hx_drv_iomux_set_pmux(IOMUX_PGPIO9, 3); //Extension Board Blue LED Output
    hx_drv_iomux_set_pmux(IOMUX_PGPIO12, 3); //Extension Board Green LED Output

    board_delay_ms(1000);
    while(1)
    {   
        sprintf(uart_buf, "Main Loop: %4d sec\r\n", timing_sec_cnt);
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);
                
        hx_drv_iomux_get_invalue(IOMUX_PGPIO1, &gpio_value);
        if(gpio_value == 1)
        {
            sprintf(uart_buf, "Get GPIO1 Logic: High\r\n\n");
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
        }
        else 
        {
            sprintf(uart_buf, "Get GPIO1 Logic: Low\r\n\n");
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
        }

        if(timing_sec_cnt & 0x01)
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 1);
        else 
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO8, 0);

        if(timing_sec_cnt & 0x02)
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 1);
        else 
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO9, 0);

        if(timing_sec_cnt & 0x04)
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 1);
        else 
            hx_drv_iomux_set_outvalue(IOMUX_PGPIO12, 0);

        timing_sec_cnt ++;
        board_delay_ms(1000);
    }
    return 0;
}
