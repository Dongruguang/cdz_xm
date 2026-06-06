#include "framework.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern cmdline_man_pt cmdline_man;
#define Color_Print 0
#define logout(x...) printf(x)

#define size_ptf_buf	128
#define max_msg				100

typedef struct{
	uint8_t type;							//描述类型
	uint8_t use_tick;					//是否使用滴答
	uint8_t reserve[2];				//保留
	char buf[ size_ptf_buf ];	//缓冲区
}pmsg_t,*pmsg_pt;

/*========================================================================*/

static thread_mq_t prt_mq;
static int mq_init_flag = 0;

static void init( void )
{
	if( 1 == mq_init_flag ) return;
	//用于向sock发送命令
	prt_mq = thread_mq_init("prt_mq", sizeof( pmsg_t ), max_msg );
	mq_init_flag = 1;
	
	cmdline_man->init();
}

static void mq_prt(char* format,...)
{
	pmsg_t pmsg;
//	pmsg.type = type;
//	pmsg.use_tick = use_tick;
	
	char *buf = pmsg.buf;
	va_list   v_args;
	va_start(v_args, format);
	(void)vsnprintf(  (char       *) buf ,
	                  (size_t      ) size_ptf_buf,
	                  (char const *) format,
	                                 v_args);
	va_end(v_args);
	
	//使用队列将打印内容发送出去
	thread_mq_send( prt_mq, &pmsg, sizeof( pmsg_t ), 0x04 );
}

extern unsigned int console_work(void);
static void work(void)
{	
	pmsg_t pmsg;
	int ptf_color = 0;
	if( mq_init_flag == 0 ) return ;
	
	cmdline_man->work();
	
	int ret = thread_mq_recv( prt_mq, &pmsg, sizeof( pmsg_t ), 0 );
	if(ret == bFALSE) return;
	
	if( 1 == Color_Print )
	switch( pmsg.type )
	{
		//标准日志
		case __type_stdlog__:
			ptf_color = 0;
			break;
		//驱动日志
		case __type_drvlog__:
			ptf_color = 1;
			logout( PRINT_FONT_GRE );
			break;
		//应用日志
		case __type_applog__:
			ptf_color = 1;
			logout( PRINT_FONT_BLA );
			break;
		//驱动错误
		case __type_drverr__:
			ptf_color = 1;
			logout( PRINT_FONT_RED );
			break;
		//应用错误
		case __type_apperr__:
			ptf_color = 1;
			logout( PRINT_FONT_RED );
			break;
		
		default:
			break;

	}
	
	if( pmsg.use_tick ) 
		logout( "[%08d]%s",get_sys_ticks(), pmsg.buf );
	else
		logout("%s",pmsg.buf);
	
	//如果有使用颜色，就取消本次显示
	if( 1 == ptf_color )
		logout( PRINT_ATTR_REC );
		
}

/*========================================================================*/
//标准日志
static void log(char* format,...)
{
	pmsg_t pmsg;
	pmsg.type 		= __type_stdlog__;
	pmsg.use_tick = 0;
	
	char *buf = pmsg.buf;
	va_list v_args;
	va_start(v_args, format);
	(void)vsnprintf( (char*)buf,(size_t)size_ptf_buf,(char const *) format,v_args );
	va_end(v_args);
	
	thread_mq_send( prt_mq, &pmsg, sizeof( pmsg_t ), 0x04 );
}

void irq_log(char* format,...)
{
	pmsg_t pmsg;
	pmsg.type 		= __type_stdlog__;
	pmsg.use_tick = 0;
	
	char *buf = pmsg.buf;
	va_list v_args;
	va_start(v_args, format);
	(void)vsnprintf( (char*)buf,(size_t)size_ptf_buf,(char const *) format,v_args );
	va_end(v_args);
	thread_mq_irq_send( prt_mq,&pmsg,sizeof( pmsg_t ),0X00 );
}
/*------------------------------------------------------------------------*/
//驱动日志
static void dlog(char* format,...)
{
	pmsg_t pmsg;
	pmsg.type 		= __type_drvlog__;
	pmsg.use_tick = 0;
	
	char *buf = pmsg.buf;
	va_list v_args;
	va_start(v_args, format);
	(void)vsnprintf( (char*)buf,(size_t)size_ptf_buf,(char const *) format,v_args );
	va_end(v_args);
	
	thread_mq_send( prt_mq, &pmsg, sizeof( pmsg_t ), 0x04 );
}
/*------------------------------------------------------------------------*/
//应用日志
static void alog(char* format,...)
{
	pmsg_t pmsg;
	pmsg.type 		= __type_applog__;
	pmsg.use_tick = 0;
	
	char *buf = pmsg.buf;
	va_list v_args;
	va_start(v_args, format);
	(void)vsnprintf( (char*)buf,(size_t)size_ptf_buf,(char const *) format,v_args );
	va_end(v_args);
	
	thread_mq_send( prt_mq, &pmsg, sizeof( pmsg_t ), 0x04 );
}
/*------------------------------------------------------------------------*/
//驱动错误
static void derr(char* format,...)
{
	pmsg_t pmsg;
	pmsg.type 		= __type_drverr__;
	pmsg.use_tick = 0;
	
	char *buf = pmsg.buf;
	va_list v_args;
	va_start(v_args, format);
	(void)vsnprintf( (char*)buf,(size_t)size_ptf_buf,(char const *) format,v_args );
	va_end(v_args);
	
	thread_mq_send( prt_mq, &pmsg, sizeof( pmsg_t ), 0x04 );
}
/*------------------------------------------------------------------------*/
//应用错误
static void aerr(char* format,...)
{
	pmsg_t pmsg;
	pmsg.type 		= __type_apperr__;
	pmsg.use_tick = 0;
	
	char *buf = pmsg.buf;
	va_list v_args;
	va_start(v_args, format);
	(void)vsnprintf( (char*)buf,(size_t)size_ptf_buf,(char const *) format,v_args );
	va_end(v_args);
	
	thread_mq_send( prt_mq, &pmsg, sizeof( pmsg_t ), 0x04 );
}

/*========================================================================*/
//标准日志
static void tlog(char* format,...)
{
	pmsg_t pmsg;
	pmsg.type 		= __type_stdlog__;
	pmsg.use_tick = 1;
	
	char *buf = pmsg.buf;
	va_list v_args;
	va_start(v_args, format);
	(void)vsnprintf( (char*)buf,(size_t)size_ptf_buf,(char const *) format,v_args );
	va_end(v_args);
	
	thread_mq_send( prt_mq, &pmsg, sizeof( pmsg_t ), 0x04 );
}
/*------------------------------------------------------------------------*/
//驱动日志
static void tdlog(char* format,...)
{
	pmsg_t pmsg;
	pmsg.type 		= __type_drvlog__;
	pmsg.use_tick = 1;
	
	char *buf = pmsg.buf;
	va_list v_args;
	va_start(v_args, format);
	(void)vsnprintf( (char*)buf,(size_t)size_ptf_buf,(char const *) format,v_args );
	va_end(v_args);
	
	thread_mq_send( prt_mq, &pmsg, sizeof( pmsg_t ), 0x04 );
}
/*------------------------------------------------------------------------*/
//应用日志
static void talog(char* format,...)
{
	pmsg_t pmsg;
	pmsg.type 		= __type_applog__;
	pmsg.use_tick = 1;
	
	char *buf = pmsg.buf;
	va_list v_args;
	va_start(v_args, format);
	(void)vsnprintf( (char*)buf,(size_t)size_ptf_buf,(char const *) format,v_args );
	va_end(v_args);
	
	thread_mq_send( prt_mq, &pmsg, sizeof( pmsg_t ), 0x04 );
}
/*------------------------------------------------------------------------*/
//驱动错误
static void tderr(char* format,...)
{
	pmsg_t pmsg;
	pmsg.type 		= __type_drverr__;
	pmsg.use_tick = 1;
	
	char *buf = pmsg.buf;
	va_list v_args;
	va_start(v_args, format);
	(void)vsnprintf( (char*)buf,(size_t)size_ptf_buf,(char const *) format,v_args );
	va_end(v_args);
	
	thread_mq_send( prt_mq, &pmsg, sizeof( pmsg_t ), 0x04 );
}
/*------------------------------------------------------------------------*/
//应用错误
static void taerr(char* format,...)
{
	pmsg_t pmsg;
	pmsg.type 		= __type_apperr__;
	pmsg.use_tick = 1;
	
	char *buf = pmsg.buf;
	va_list v_args;
	va_start(v_args, format);
	(void)vsnprintf( (char*)buf,(size_t)size_ptf_buf,(char const *) format,v_args );
	va_end(v_args);
	
	thread_mq_send( prt_mq, &pmsg, sizeof( pmsg_t ), 0x04 );
}

static uart_pt dbg_uart;

static void set_uart( uart_pt uart )
{
	dbg_uart = uart;
}

static uart_pt prot(void)
{
	return dbg_uart;
}


/*========================================================================*/
static sysprt_t do_sysprt={
	init,
	work,
	
	log,dlog,alog,derr,aerr,
	
	tlog,tdlog,talog,tderr,taerr,

	set_uart,prot
};

sysprt_pt sysprt = &do_sysprt;





