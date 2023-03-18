#include "main.h"

// #include "hx_drv_iic_m.h"
// #include "hx_drv_iomux.h"
#include "SC16IS750_Bluepacket.h"

DEV_UART * uart0_ptr;
char uart_buf[uart_buf_size] = {0};

volatile uint8_t gpio_out_state = 0;
volatile uint8_t gpio_in_value;

int main(void)
{    
	//UART 0 is already initialized with 115200bps
	printf("This is Lab2_SPI_Arduino_GPIO752\r\n");

    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);

	HX_GPIOSetup();
	IRQSetup();
	UartInit(SC16IS750_PROTOCOL_SPI);
	
	GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, OUTPUT);
	GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6, INPUT);
	while(1)
	{
		GPIOSetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, gpio_out_state);
		gpio_in_value = GPIOGetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO6);

		if(gpio_out_state == 1)
        {
            sprintf(uart_buf, "Get 752_GPIO[7] Logic: High\r\n\n");
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
        }
        else 
        {
            sprintf(uart_buf, "Get 752_GPIO[7] Logic: Low\r\n\n");
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
        }

		if(gpio_in_value == 1)
        {
            sprintf(uart_buf, "Get 752_GPIO[6] Logic: High\r\n\n");
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
        }
        else 
        {
            sprintf(uart_buf, "Get 752_GPIO[6] Logic: Low\r\n\n");
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
        }

		if(gpio_out_state == 0)
			gpio_out_state = 1;
		else 
			gpio_out_state = 0;

		board_delay_ms(1000);
	}

	return 0;
}
