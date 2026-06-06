#include "task_inc.h"
#include "framework.h"
#include <stdlib.h>
#include "work.h"
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


*/



static void init_io_pin(void)
{
	gpio->set_mode( IO_SLED, PIN_MODE_OUTPUT );
	gpio->set_mode( IO_NetRst, PIN_MODE_OUTPUT );	
	
	gpio->set_mode( IO_ELC0, PIN_MODE_OUTPUT );
	gpio->set_mode( IO_ELC1, PIN_MODE_OUTPUT );
	
	gpio->set_mode( IO_fuse0, PIN_MODE_INPUT );
	gpio->set_mode( IO_fuse1, PIN_MODE_INPUT );

//	gpio->set_mode( Hight0, PIN_MODE_INPUT );
//	gpio->set_mode( Hight1, PIN_MODE_INPUT );
	
	gpio->write( IO_ELC0, PIN_LOW );
	gpio->write( IO_ELC1, PIN_LOW );
	
}
//-------------------------------------------------------------------------

static void JumpToApp(uint32_t addr)
{	
	__disable_irq();
	
	uint32_t sp = *((volatile uint32_t *)(addr));
	uint32_t pc = *((volatile uint32_t *)(addr + 4));
	
	typedef void (*Func_void_void)(void);
	Func_void_void ResetHandler = (Func_void_void)pc;
	
	SCB->VTOR = addr;
	
	__set_MSP(sp);
	
	ResetHandler();
	
	while(1) __NOP();
}

static void deinit(void)
{
	SysTick->CTRL = 0;
	SysTick->VAL=0x00;
	uart0->stop( );
//	uart1->stop( );
	NVIC_DisableIRQ(UART0_IRQn);
	NVIC_DisableIRQ(UART1_IRQn);
	
	
}



static void run_by_config( void )
{
	switch( wdat->storage->boot_app )
	{
		case _app_update:
			sysprt->alog("-----> _app_update \r\n");
			break;
		
		case _app_fac:
			sysprt->alog("-----> _app_fac \r\n");
			thread_delay_ms( 500 );
		
			deinit();
			
			JumpToApp( addr_app0 );
			break;
		case _app_new:
			sysprt->alog("-----> _app_new \r\n");
			thread_delay_ms( 500 );
			
			deinit();
			JumpToApp( addr_app1 );
			break;
	}
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

	dot_ui->init();
	dot_ui->show_ascii_str(0,0,"Boot Loader");
	dot_ui->show();

	printf( "build:%s-%s\r\n",build_date,build_time );
	
	sprintf( vbuf,"%d_%d-%d_%d",
		Brd_Version,Brd_SubVersion,
		BL_Version,BL_SubVersion
	);
	
	printf( "version:%s\r\n",vbuf );
	
	printf("\r\n");
	for(int i=0;i<LCD_XPixel;i++)	printf("-");
	printf("\r\n");
	
	printf("SystemCoreClock = %d\r\n",SystemCoreClock);
	
	
}
//-------------------------------------------------------------------------
void swap_uart0_to_uart1(void)
{
	uint8_t rxbuf[8];
	int ret = uart0->read_recv_buf( rxbuf,1 );
	if( ret <=0 ) return;	
	
	uart1->send( rxbuf,ret );
//	printf("uart0 rx:%c\r\n",rxbuf[0]);
}

void swap_uart1_to_uart0(void)
{
	uint8_t rxbuf[8];
	int ret = uart1->read_recv_buf( rxbuf,1 );
	if( ret <=0 ) return;	
	
//	printf("uart1 rx:%c\r\n",rxbuf[0]);
	uart0->send( rxbuf,ret );
}


//-------------------------------------------------------------------------
extern void air724_rst(void);
static void task_entry(void *arg)
{
	
#if 0
	init_io_pin();
	hc595_opt->init( LED_BUS );
	uart0->init( 115200 );
	uart1->init( 115200 );
	gpio->write( 56,0 );
	task_delay(100);
	gpio->write( 56,1 );
	task_delay(100);
	gpio->write( 56,0 );
	task_delay(100);
	
	printf("run...\r\n");
	
	task_delay(4000);
	while(1)
	{
		swap_uart0_to_uart1();
		swap_uart1_to_uart0();
		
		vTaskDelay(1);
	}
#endif
	//初始化外设
	sysprt->init();
	sysprt->set_uart( uart0 );
	
	
	uart0->init( 115200 );
	hc595_opt->init( LED_BUS );
	
	init_io_pin();
	//显示软件信息
	show_version_info();
	
	
	//host任务负责调试相关
	task_host->create(NULL,NULL);
	
	
	thread_delay_ms(100);
	
	work_data_load();							//加载工作数据
	cmdline_man->boot_wait();

	run_by_config();
	
	
	uart1->init( 115200 );

	sysprt->alog("\r\nrun...\r\n");
	

	
	//-------------------------------------------------------------------------
	//下面开启各种服务
	
	//创建listen任务
//	task_listen->create(NULL,NULL);
	
	//创建ctrl任务
	task_ctrl->create( NULL,NULL );
	
	vTaskDelay(100);
	
	ota_wdt_start();
	sysprt->alog("ota...\r\n");

	while(1)
	{
		if( 
			( wdat->storage->net_mode == NetMode_Air724 ) || 
			( wdat->storage->net_mode == NetMode_Air780 )
		)
		{
			work_internet();
		}
		
		vTaskDelay(1);
		
	}
}


__task_body_end;

//将本文件的task_body开放给别的文件引用
__task_body_quote_to( task_start );










