#include "task_inc.h"
#include "framework.h"
#include <stdlib.h>
#include "work.h"

__task_body_start;


static task_body_t m_task_body={
	.name		= "ctrl_task",
	.cn_name	= "控制任务",
	.stack_size	= 2048,
	.priority = 3,
	.handle = &handle,
	.create = create,
	
}; 
/*----------------------------------------------------------------------------------*/

static int req_rst = 0;


	


void req_rst_mcu( void )
{
	save_order_them_shutdown();
	req_rst = 1;
}
/*----------------------------------------------------------------------------------*/
static int ota_sta = 0;
static time_t otick_ota = 0;
static time_t ntick_ota = 0;

void req_goto_update( void )
{
	ota_sta = 1;
}
static void deal_ota_work( void )
{
	
	switch( ota_sta )
	{
		case 0:
			
			break;
		
		case 1:
			work_data_write();
			otick_ota = get_sys_ticks();
			sysprt->alog( "==================>goto ota after 5s\r\n" );
			ota_sta = 2;
			break;
		case 2:
			ntick_ota = get_sys_ticks();
			if ( ( ntick_ota - otick_ota ) < 5000 ) break;
			
			req_rst_mcu();
			break;
		default:break;
	}
}
/*----------------------------------------------------------------------------------*/


static void task_entry(void *arg)
{
	/*----------------------------------------------------------------------------------*/
	sysprt->alog("run task_entry: %s\r\n",m_task_body.name);
	sock_init();
	led_board->init();
	
	sysprt->dlog("mem_get_free = %010u KB\r\n",mem_get_free()/1024 );

	
#ifdef 	WDT_FUNC
	WDT_Init(WDT, SystemCoreClock, WDT_MODE_RESET);	//每秒需要喂狗一次
	WDT_Start(WDT);																		//启动WDT
#endif
	
	while(1)
	{
#ifdef 	WDT_FUNC
		if( 0 == req_rst )
			WDT_Feed( WDT );																//喂狗
#endif
		
		deal_ota_work();
		sock_work_entry();
		vTaskDelay(1);
	}
}

__task_body_end;


//将本文件的task_body开放给别的文件引用
__task_body_quote_to( task_ctrl );

