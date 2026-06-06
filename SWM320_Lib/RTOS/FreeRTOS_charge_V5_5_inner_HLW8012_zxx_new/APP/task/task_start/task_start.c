#include "task_inc.h"
#include "framework.h"
#include <stdlib.h>
#include "work.h"
#include "message.h"

#include <flashdb.h>

__task_body_start;


static task_body_t m_task_body={
	.name		= "start_task",
	.cn_name	= "起始任务",
	.stack_size	= 4096,
	.priority = 3,
	.handle = &handle,
	.create = create,
	
}; 





/*
(1)UART驱动，波特率/晶振适配
(2)PWM驱动		
(3)编码器驱动	
(4)OUTPIN驱动	
(5)ADC驱动

行为管理器


工作逻辑
(1)订单开始后，3分钟之内，不判断充满，只判断拔电
(2)开始订单，有40s时间的等待期，	
	这40s也是统计电能的，但是不判断是否拔电/未插入用电器。
(3)拔电的判断方式,等了一段时间(2s)都没有等到下降沿，则认为被拔电了
(4)充满的判断标准，1分钟充电电能，低于15W*60s 



*/




static void init_io_pin(void)
{
	

	gpio->set_mode( IO_SLED, PIN_MODE_OUTPUT );
	gpio->set_mode( IO_NetRst, PIN_MODE_OUTPUT );	
	
	gpio->set_mode( IO_ELC0, PIN_MODE_OUTPUT );
	gpio->set_mode( IO_ELC1, PIN_MODE_OUTPUT_OD );
	
	gpio->set_mode( IO_fuse0, PIN_MODE_INPUT );
	gpio->set_mode( IO_fuse1, PIN_MODE_INPUT );

	
	gpio->write( IO_ELC0, PIN_LOW );
	gpio->write( IO_ELC1, PIN_LOW );
	
}



//-------------------------------------------------------------------------

char* build_date = __DATE__;
char* build_time = __TIME__;


static void show_version_info(void)
{
	char vbuf[30];
	
	printf("\r\n");
	for(int i=0;i<LCD_XPixel;i++)	printf("-");
	printf("\r\n");
	
//	dot_ui->init();
//	dot_ui->show_ascii_str(0,0,"new app");
//	dot_ui->show();

	printf( "build:%s-%s\r\n",build_date,build_time );
	
	sprintf( vbuf,"%d_%d-%d_%d",
		Brd_Version,Brd_SubVersion,
		App_Version,App_SubVersion
	);
	
	printf( "version:%s\r\n",vbuf );
	
	printf("\r\n");
	for(int i=0;i<LCD_XPixel;i++)	printf("-");
	printf("\r\n");
	
	printf("SystemCoreClock = %d\r\n",SystemCoreClock);
}

//上电先随便初始化一遍RTC
static void init_rtc(void)
{
	rtc_dt.Year = 2024;
	rtc_dt.Month = 10;
	rtc_dt.Date = 10;
	rtc_dt.Hour = 10;
	rtc_dt.Minute = 10;
	rtc_dt.Second = 10;
	
	rtc->stop();
	rtc->init( &rtc_dt,0,0 );
	rtc->start();
	
}

static void task_entry(void *arg)
{
	//初始化外设
	sysprt->init();
	uart0->init( 115200 );
	
	init_io_pin();
	
	
	//显示软件信息
	show_version_info();
	
	//host任务负责调试相关
	task_host->create(NULL,NULL);
	
	
	
	vTaskDelay(100);
	
	wdat_guide_init();
	sysprt->alog("~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n" );
	
	init_rtc();
	
	work_data_load();							//加载工作数据
	cmdline_man->boot_wait();
	
	sysprt->alog("run...\r\n");
	uart1->init( 115200 );
	

	hc595_opt->init( LED_BUS );
	
	
	sock_hlw_opt->init();
	ipool->init();
	
	//-------------------------------------------------------------------------
	//下面开启各种服务
	
	//创建listen任务
	task_listen->create(NULL,NULL);
//	while(1);
	
	//创建ctrl任务
	task_ctrl->create( NULL,NULL );
	
	vTaskDelay(100);
	
	

	while(1)
	{
		if( 
			( wdat->storage->net_mode == NetMode_Air724 ) || 
			( wdat->storage->net_mode == NetMode_Air780 )
		)
		{
			//work_internet();
			network_entry();
		}
		
		vTaskDelay(5);
		
	}
}


__task_body_end;

//将本文件的task_body开放给别的文件引用
__task_body_quote_to( task_start );









