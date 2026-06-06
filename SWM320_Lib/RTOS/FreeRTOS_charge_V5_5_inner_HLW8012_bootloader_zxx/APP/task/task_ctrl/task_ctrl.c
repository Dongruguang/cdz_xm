#include "task_inc.h"
#include "framework.h"
#include <stdlib.h>
#include "work.h"

__task_body_start;


static task_body_t m_task_body={
	.name		= "ctrl_task",
	.cn_name	= "控制任务",
	.stack_size	= 512,
	.priority = 3,
	.handle = &handle,
	.create = create,
	
}; 
/*----------------------------------------------------------------------------------*/
#define WDT_FUNC	//关掉此宏，调试debug时，不会因不为苟而 自动重启。
static int req_rst = 0;
void req_rst_mcu( void )
{
	req_rst = 1;
}
/*----------------------------------------------------------------------------------*/
static time_t otick_ota = 0;
static time_t ntick_ota = 0;
static int ota_wdt_sta = 0;

void ota_wdt_start( void )
{
	otick_ota = get_sys_ticks();
	ota_wdt_sta = 1;						//进入监听模式
}

uint32_t ota_wdt_tick( void )
{
	time_t use_tick = 0;
	time_t ntick = get_sys_ticks();
	use_tick = ntick - otick_ota;
	return use_tick/1000;
}
static void ota_wdt_listen( void )
{
	switch( ota_wdt_sta )
	{
		//等待开启
		case 0:
			break;
		//正在监听
		case 1:
			ntick_ota = get_sys_ticks();
			if(( ntick_ota - otick_ota ) > (OTA_TimeOut*1000) )
				ota_wdt_sta++;
			break;
		//监听超时,停止喂狗
		case 2:
			req_rst_mcu();	
			ota_wdt_sta++;
			break;
		//等待看门狗超时
		case 3:
			break;
		default:
			break;
	}
	
}

/*----------------------------------------------------------------------------------*/

static void task_entry(void *arg)
{
	/*----------------------------------------------------------------------------------*/
	sysprt->alog("run task_entry: %s\r\n",m_task_body.name);
	led_board->init();
	
	sysprt->dlog("mem_get_free =%08dKB",mem_get_free()/1024 );

#ifdef 	WDT_FUNC
	WDT_Init(WDT, SystemCoreClock, WDT_MODE_RESET);	//每0.5秒需要喂狗一次
	WDT_Start(WDT);																		//启动WDT
#endif
	
	while(1)
	{
		
#ifdef 	WDT_FUNC
		if( 0 == req_rst )	WDT_Feed( WDT );																//喂狗
#endif
		
		ota_wdt_listen();
		vTaskDelay(10);
		
	}
}

__task_body_end;


//将本文件的task_body开放给别的文件引用
__task_body_quote_to( task_ctrl );

