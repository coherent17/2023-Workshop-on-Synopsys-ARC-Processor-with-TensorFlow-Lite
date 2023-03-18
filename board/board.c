/**************************************************************************************************
    (C) COPYRIGHT, Himax Technologies, Inc. ALL RIGHTS RESERVED
    ------------------------------------------------------------------------
    File        : board.c
    Project     : WEI
    DATE        : 2018/10/01
    AUTHOR      : 902452
    BRIFE       : main function
    HISTORY     : Initial version - 2018/10/01 created by Will
**************************************************************************************************/
#include "embARC.h"
#include "embARC_debug.h"
#ifdef EVENT_HANDLER_LIB
#include "event.h"
#endif
#include "board.h"
uint32_t g_bsp_version = 0;
//extern uint32_t g_pad_dir_78;
//extern uint32_t g_pad_dir_7c;

typedef struct main_args {
	int argc;
	char *argv[];
} MAIN_ARGS;

/** Change this to pass your own arguments to main functions */
MAIN_ARGS s_main_args = {1, {"main"}};

#if defined(EMBARC_USE_BOARD_MAIN)
/*--- When new embARC Startup process is used----*/
#define MIN_CALC(x, y)		(((x)<(y))?(x):(y))
#define MAX_CALC(x, y)		(((x)>(y))?(x):(y))

#ifdef OS_FREERTOS
/* Note: Task size in unit of StackType_t */
/* Note: Stack size should be small than 65536, since the stack size unit is uint16_t */
#define MIN_STACKSZ(size)	(MIN_CALC(size, configTOTAL_HEAP_SIZE) / sizeof(StackType_t))

#ifndef TASK_PRI_MAIN
#define TASK_PRI_MAIN		1	/* Main task priority */
#endif
static TaskHandle_t task_handle_main;

#endif /* OS_FREERTOS */

static void task_main(void *par)
{
	int ercd;

	if ((par == NULL) || (((int)par) & 0x3)) {
	/* null or aligned not to 4 bytes */
		ercd = _arc_goto_main(0, NULL);
	} else {
		MAIN_ARGS *main_arg = (MAIN_ARGS *)par;
		ercd = _arc_goto_main(main_arg->argc, main_arg->argv);
	}

#if defined(OS_FREERTOS)
	EMBARC_PRINTF("Exit from main function, error code:%d....\r\n", ercd);
	while (1) {
		vTaskSuspend(NULL);
	}
#else
	while (1);
#endif
}

void board_main(void)
{
/* board level hardware init */
	board_init();
/* board level middlware init */

	timer_calibrate_delay(BOARD_CPU_CLOCK);

#ifdef LIB_COMMON
	xprintf_setup();
#endif

    infra_evts_init();//need to erase later

#ifdef ENABLE_OS
	os_hal_exc_init();
#endif

#ifdef OS_FREERTOS
	xTaskCreate((TaskFunction_t)task_main, "main", TASK_STACK_SIZE_MAIN,
			(void *)(&s_main_args), TASK_PRI_MAIN, &task_handle_main);
#else /* No os and ntshell */
	cpu_unlock();	/* unlock cpu to let interrupt work */
	task_main((void *)(&s_main_args));
#endif

#ifdef OS_FREERTOS
	vTaskStartScheduler();
#endif

/* board level exit */
}
uint32_t hx_board_get_version(void)
{
	return g_bsp_version;
}

#else /*-- Old embARC Startup process */

static void enter_to_main(MAIN_ARGS *main_arg)
{
	if (main_arg == NULL) {
	/* null or aligned not to 4 bytes */
		_arc_goto_main(0, NULL);
	} else {
		_arc_goto_main(main_arg->argc, main_arg->argv);
	}
}


void board_main(void)
{

#ifndef PURE_BOARD_INIT
#if HW_VERSION == 23
	uint32_t reg_val = 0;
	// ahb clk source select
	reg_val = _arc_read_uncached_32((void *)0xb0000010);
	reg_val = reg_val | 0x04;
	_arc_write_uncached_32((void *)0xb0000010, reg_val);

#if 0 // disable this - RD update default setting : select dfss uart
	reg_val = _arc_read_uncached_32((void *)0xb0010110);
	reg_val = reg_val | 0x80000000;
	_arc_write_uncached_32((void *)0xb0010110, reg_val);
#endif
#endif
	
	/* board level hardware init */
	board_init();

	/* Library Support List */
#ifdef LIB_COMMON
	xprintf_setup();
#endif
//	xprintf("BSP Version: 0x%08x\r\n", g_bsp_version);
//	xprintf("g_pad_dir_78=0x%08x\r\n", g_pad_dir_78);
//	xprintf("g_pad_dir_7c=0x%08x\r\n", g_pad_dir_7c);
#ifdef EVENT_HANDLER_LIB
	infra_evts_init();
#endif
#else

#ifndef FPGA_PLATFORM
	specific_board_init(BOARD_INIT_SPEC_PERI_I2CM | BOARD_INIT_SPEC_PERI_UART | BOARD_INIT_SPEC_PERI_SPIM |
			BOARD_INIT_SPEC_PERI_GPIO | BOARD_INIT_SPEC_PWM | BOARD_INIT_SPEC_DP );
#endif

#endif
	enter_to_main(&s_main_args);
}

uint32_t hx_board_get_version(void)
{
	return g_bsp_version;
}
#endif /* EMBARC_USE_BOARD_MAIN */
