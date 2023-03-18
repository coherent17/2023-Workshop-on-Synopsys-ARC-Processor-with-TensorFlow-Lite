#include "main.h"

#include "aud_lib.h"

DEV_UART * uart0_ptr;
char uart_buf[uart_buf_size] = {0};

#define mic_sample_rate 16000
#define AUD_BLK_STEP 16	//
#define AUD_WAIT_STEP 0
#define AUD_BLK_SZ 1024	//0.064sx`
#define AUD_BLK_STEP_SZ (AUD_BLK_SZ * AUD_BLK_STEP)	//1.024s = 0.064 * 16

#define AUD_STEP_CNT 5	//Record time: X * 1.024s
#define AUD_BUF_SIZE (AUD_BLK_STEP_SZ * AUD_STEP_CNT)	//Record time: 10 * 1024ms

#define AUD_TIME_MS 1024	//1024ms

typedef struct {
	int16_t left;
	int16_t right;
} META_AUDIO_t;

volatile META_AUDIO_t audio_clip_stamp[AUD_BLK_STEP_SZ] = {0};
volatile META_AUDIO_t audio_clip[AUD_BUF_SIZE] = {0};
int audio_flag = 0;


uint32_t read_test = 0;
uint32_t wrap_test = 0;
audio_config_t aud_pdm_cfg;

uint32_t record_step_cnt = 0;


void pdm_rx_callback_fun(uint32_t status);

int main(void)
{
    //UART 0 is already initialized with 115200bps
    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);

    sprintf(uart_buf, "This is Lab3_PDM_Microphone\r\n");    
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(10);    

    sprintf(uart_buf, "Microphone Initialize\r\n");    
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(10);    

    hx_lib_audio_set_if(AUDIO_IF_PDM);
    hx_lib_audio_init();
    audio_flag = 0;

    hx_lib_audio_register_evt_cb(pdm_rx_callback_fun);
  
    aud_pdm_cfg.sample_rate = AUDIO_SR_16KHZ;
    aud_pdm_cfg.buffer_addr = (uint32_t *) (0x20000000+36*1024);//0x20009000;
    aud_pdm_cfg.block_num = (AUD_BLK_STEP + AUD_WAIT_STEP);
    aud_pdm_cfg.block_sz = AUD_BLK_SZ * 4;
    aud_pdm_cfg.cb_evt_blk = 2;  

    sprintf(uart_buf, "Wait for user press key: [A] \r\n");    
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(10);    

    while(1)
    {
        uint8_t user_key = 0;
        uint8_t function_ret = uart0_ptr->uart_read_nonblock(&user_key, 1);
        if(function_ret != 1)
            user_key = 0;

        if(user_key == 'A')
        {            
            sprintf(uart_buf, "Total audio length = %d sec\r\n", ((uint32_t) (1.024 * AUD_STEP_CNT)));    
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);    
            sprintf(uart_buf, "Microphone start running in the background\r\n");    
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);    
            hx_lib_audio_start(&aud_pdm_cfg);

            for(record_step_cnt = 0; record_step_cnt < AUD_STEP_CNT; record_step_cnt ++)
            {
                while(audio_flag == 0)
                {
                    board_delay_ms(1);     
                }
                audio_flag = 0;

                memcpy((void *)(audio_clip + (record_step_cnt * AUD_BLK_STEP_SZ)), (void *)(audio_clip_stamp), (AUD_BLK_STEP_SZ * 4));
                
                sprintf(uart_buf, "Step: %d\r\n", record_step_cnt);   
                uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                    board_delay_ms(5);
            }
            hx_lib_audio_stop();

            sprintf(uart_buf, "Microphone Get Data Success\r\n");   
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);
                
            sprintf(uart_buf, "Start to send\r\n");    
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);

            for(uint32_t i = 0; i < AUD_BUF_SIZE; i ++)
            {
                //This is type 1 string
                //sprintf(uart_buf, "%d, %d, %d\r\n", i, audio_clip[i].left, audio_clip[i].right);

                //This is type 2 string
                sprintf(uart_buf, "%d,%d\r\n", audio_clip[i].left, audio_clip[i].right);

                
                uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                    board_delay_ms(1);
            }

            sprintf(uart_buf, "End of send\r\n");    
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);

            sprintf(uart_buf, "Wait for user press key: [A] \r\n");    
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
                board_delay_ms(10);    
        }

        board_delay_ms(10);
    }
}

void pdm_rx_callback_fun(uint32_t status)
{
    static uint32_t last_block = 0;
    static uint32_t stamp_idx = 0;
    static uint32_t pdm_idx = 0;
    static uint8_t first_flag = 0;

    uint32_t pdm_buf_addr;
    uint32_t block;
    uint32_t data_size;
    uint32_t audio_size;

    hx_lib_audio_request_read(&pdm_buf_addr, &block);
    // block = block + 1;

    if(last_block > block)
    {
        audio_size = ((AUD_BLK_STEP + AUD_WAIT_STEP) - last_block) << 10;
        data_size = audio_size << 2;

        memcpy((void *) (audio_clip_stamp + stamp_idx), (void *) (pdm_buf_addr + pdm_idx), data_size);

        pdm_idx = 0;
        stamp_idx = 0;
        last_block = 0;

        audio_flag = 1;
    }
    else
    {
        audio_size = (block - last_block) << 10;
        data_size = audio_size << 2;

        memcpy((void *) (audio_clip_stamp + stamp_idx), (void *) (pdm_buf_addr + pdm_idx), data_size);

        stamp_idx = stamp_idx + audio_size;
        pdm_idx = pdm_idx + data_size;
        last_block = block;
    }

/*
    block: 2 means 0~1 block is finished.
    block 3~15 is saved 0~14 blocks
    next loop block 2 save 15 block and set up finish flag

    sprintf(uart_buf, "addr:0x%08x, block_num:%d, audio_size:%d, data_size:%d \r\n", pdm_buf_addr, block, audio_size, (pdm_idx/4));
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));    
*/
    return;
}
