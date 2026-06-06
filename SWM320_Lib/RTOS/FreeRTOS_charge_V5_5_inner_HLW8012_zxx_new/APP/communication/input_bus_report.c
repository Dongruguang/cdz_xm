

#include "task_inc.h"
#include "framework.h"

#include "communication.h"

static int en_cycle_report = __Disable;				//描述定时汇报 功能开关


//定时汇报 功能开关
static void cycle_report_set(uint8_t enable)
{
	if( enable == __Disable )
	{
		en_cycle_report = __Disable;
	}
	else
	{
		en_cycle_report = __Enable;
	}
}

//定时汇报 执行函数
static uint8_t pin[8];
static 	time_t otick=0,ntick=0;
static void cycle_report(time_t tick)
{
}


/*----------------------------------------------------------------------------------*/
static uint8_t conf_event[64];							//描述配置的事件时什么
//事件汇报 功能开关
static void event_report_set(uint8_t pin,uint8_t event)
{
	int i = 0;
	
	if( pin == 0XFF )
	{//设置全部
		for( i=0;i<64;i++ )
			conf_event[i] = event;
		return;
	}
	if( pin <64  )
	{//正常处理
		conf_event[i] = event;
	}

}


static uint8_t nevent_pin[8]={0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF};
static uint8_t oevent_pin[8]={0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF};
static uint8_t sbuf[8];

//事件汇报 执行函数
static void event_report(void)
{
}


static inb_report_t do_inb_report ={
	.cycle_report_set = cycle_report_set,
	.cycle_report = cycle_report,
	.event_report_set = event_report_set,
	.event_report = event_report,
};

inb_report_pt inb_report=&do_inb_report;



