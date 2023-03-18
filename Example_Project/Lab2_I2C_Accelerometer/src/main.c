#include "main.h"

#include "hx_drv_iic_m.h"
#include "synopsys_sdk_GMA303KU.h"

DEV_UART * uart0_ptr;
char uart_buf[uart_buf_size] = {0};

DEV_IIC * iic1_ptr;

int16_t accel_x;
int16_t accel_y;
int16_t accel_z;
int16_t accel_t;

uint8_t xg_sign;
uint8_t yg_sign;
uint8_t zg_sign;
uint8_t temp_sign;

int16_t xg_10x;
int16_t yg_10x;
int16_t zg_10x;
int16_t temp_10x;

int main(void)
{    
    //UART 0 is already initialized with 115200bps
    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);

    sprintf(uart_buf, "This is Lab2_I2C_Acccelerometer\r\n");
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(10);

    iic1_ptr = hx_drv_i2cm_get_dev(USE_SS_IIC_0);
    iic1_ptr->iic_open(DEV_MASTER_MODE, IIC_SPEED_STANDARD); 

    sprintf(uart_buf, "Accle Init Start\r\n");    
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(10);

    uint8_t chip_id = GMA303KU_Init();
    board_delay_ms(100);

    if(chip_id == 0xA3)
        sprintf(uart_buf, "Chip ID: 0x%2X | OK\r\n\n", chip_id);    //It should be 0xA3
    else 
        sprintf(uart_buf, "Chip ID: 0x%2X | Error\r\n\n", chip_id);    //It should be 0xA3
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
    board_delay_ms(10);

    while(1)
    {
        uint16_t reg_04_data = GMA303KU_Get_Data(&accel_x, &accel_y, &accel_z, &accel_t);

        if(reg_04_data == 0x0055)
            sprintf(uart_buf, "Reg 04: 0x%04X | OK\r\n", reg_04_data);
        else     
            sprintf(uart_buf, "Reg 04: 0x%04X | Error\r\n", reg_04_data);    
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);


        accel_x = accel_x / 2;
        accel_y = accel_y / 2;

        xg_10x = accel_x * 10 / 256;
        yg_10x = accel_y * 10 / 256;
        zg_10x = accel_z * 10 / 256;
        temp_10x = 250 + accel_t * 5;

        if(xg_10x >= 0)
            xg_sign = '+';
        else 
            xg_sign = '-';

        if(yg_10x >= 0)
            yg_sign = '+';
        else 
            yg_sign = '-';

        if(zg_10x >= 0)
            zg_sign = '+';
        else 
            zg_sign = '-';

        if(temp_10x >= 0)
            temp_sign = '+';
        else 
            temp_sign = '-';

        sprintf(uart_buf, "X: %+6d | %c%2d.%01d[G]\r\n", accel_x, xg_sign, abs(xg_10x / 10), abs(xg_10x % 10));    
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);
        
        sprintf(uart_buf, "Y: %+6d | %c%2d.%01d[G]\r\n", accel_y, yg_sign, abs(yg_10x / 10), abs(yg_10x % 10));    
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);
        
        sprintf(uart_buf, "Z: %+6d | %c%2d.%01d[G]\r\n", accel_z, zg_sign, abs(zg_10x / 10), abs(zg_10x % 10));    
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);

        sprintf(uart_buf, "Temp: %+3d | %c%3d.%01d[C]\r\n", accel_t, temp_sign, abs(temp_10x / 10), abs(temp_10x % 10));    
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);

        sprintf(uart_buf, "--------------------------\r\n\n");   
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(10);

        board_delay_ms(1000);
    }
    return 0;
}
