#include "main.h"

#include "add_ab.h"
#include "helloworld.h"

DEV_UART * uart0_ptr;
char uart_buf[uart_buf_size] = {0};
int main(void)
{
    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);
    uart0_ptr->uart_open(UART_BAUDRATE_115200); //UART0 can't change baud

    sprintf(uart_buf, "This is Lab0_c_with_cpp_helloworld\r\n");
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(1000);

    sprintf(uart_buf, "Main\r\n");
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(10);

    helloworld();

    int a = 1;
    int b = 2;
    int c = add_user(a, b);

    sprintf(uart_buf, "Test: %d + %d = %d\r\n", a, b, c);
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(1000);
    return 0;
}
