#include "main.h"

DEV_UART * uart0_ptr;
DEV_UART * uart1_ptr;

char uart_buf[uart_buf_size] = {0};
char str_buf[uart_buf_size] = {0};
char char_buf[10] = {0};
uint8_t char_cnt;
uint8_t uart_enter_flag;

int function_ret;

int main(void)
{

    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);
    uart1_ptr = hx_drv_uart_get_dev(USE_SS_UART_1);

    //set baud rate (bps)
    uart0_ptr->uart_open(UART_BAUDRATE_115200); //UART0 can't change baud
    uart1_ptr->uart_open(UART_BAUDRATE_57600);

    //UART send data
    sprintf(uart_buf, "This is Lab1_UART\r\n");
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
    uart1_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(1000);

    while(1){

        //return how many bytes it gets but no greater than 1
        function_ret = uart0_ptr->uart_read_nonblock(&char_buf, 1);
        if(function_ret == 1)
        {
            sprintf(uart_buf, "Echo char: 0x%02x\r\n", char_buf[0]);
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);      

            sprintf(uart_buf, "String cnt: %d\r\n\n", char_cnt);
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);      

            str_buf[char_cnt] = char_buf[0];

            char_cnt ++;
            if(char_cnt >= uart_buf_size)
                char_cnt = uart_buf_size - 1;

            if(char_buf[0] == 0x0d)
            {
                uart_enter_flag = 1;
                str_buf[char_cnt] = '\0';
                char_cnt = 0;
            }   

            if(uart_enter_flag == 1)
            {
                uart_enter_flag = 0;
                sprintf(uart_buf, "\n\n\nEcho string: %s\r\n\n\n", str_buf);
                uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                    board_delay_ms(10);                     
            }
        }
        board_delay_ms(1);
    }
    return 0;
}
