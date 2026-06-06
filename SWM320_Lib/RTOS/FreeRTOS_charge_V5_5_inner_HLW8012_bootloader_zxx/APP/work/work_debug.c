#include "work.h"

#define PRT_LOG(X...)	sysprt->alog(X)	

static void save_data(void)
{
	PRT_LOG("save_data..." );
	work_data_write();
	PRT_LOG("===>OK\r\n" );
}
/*-------------------------------------------------------------------------*/
#define check_argc_error( ival,jump ) \
	do{ \
		if( argc <ival ){ \
			PRT_LOG( "err: argc < %d \r\n",ival ); \
			goto jump; \
		} \
	}while(0);

static int task_config( char argc, char **argv )
{
	uint32_t val = 0;
//	double ratio = 0; 
	
	check_argc_error( 2,__usage_cmd );
	

	if( 0 == strcmp( argv[1],"ratio" ) )
	{//电流系数
		goto __deal_app_cmd;
	}
	else if( 0 == strcmp( argv[1],"rst_kb" ) )
	{//复位电流系数 
		goto __deal_app_cmd;
	}
	else if( 0 == strcmp( argv[1],"kb" ) )
	{//设置拟合直线系数 k和b
		goto __deal_app_cmd;
	}
	else if( 0 == strcmp( argv[1],"store" ) )
	{//存储配置
		check_argc_error( 2,__usage_store );
		goto __deal_store;
	}
	else if( 0 == strcmp( argv[1],"net_ptf" ) )
	{//配置 网络打印
		check_argc_error( 3,__usage_net_ptf );
		goto __deal_net_ptf;
	}
	else if( 0 == strcmp( argv[1],"default" ) )
	{////配置数据复位
		check_argc_error( 2,__usage_default );
		goto __deal_default;
	}
	else if( 0 == strcmp( argv[1],"net" ) )
	{////配置上网方式
		check_argc_error( 3,__usage_net );
		goto __deal_net;
	}

	
	
	return 0;
//------------------------------------------------------------------	
__usage_cmd:
	PRT_LOG("->config <cmd>\r\n");
	PRT_LOG("cmd:zd_val | ratio | hlw_ptf | net_ptf | store | default | net\r\n" );
	return -1;
//------------------------------------------------------------------	

__deal_app_cmd:
	
	PRT_LOG("Please goto run app,then config\r\n");
	return 0;

//------------------------------------------------------------------	
		
__usage_store:
	
	return -2;
__deal_store:
	PRT_LOG("config store \r\n" );
	save_data();
	return 0;
//------------------------------------------------------------------	

//------------------------------------------------------------------	

__usage_net_ptf:
	
	return -2;
__deal_net_ptf:
	val = atol( argv[2] );
	wdat->storage->net_ptf = val;
	PRT_LOG("config net_ptf -->%d \r\n",val );
	return 0;
	
//------------------------------------------------------------------	
__usage_default:
	
	return -2;
__deal_default:
	wdat->storage->boot_code = 0;
	PRT_LOG("config resrt\r\n",val );
	save_data();
	return 0;
//------------------------------------------------------------------	
__usage_net:
	
	return -2;
__deal_net:
	if( 0 == strcmp( argv[2],"air724" ) )
	{
		wdat->storage->net_mode = NetMode_Air724;
	}
	else if( 0 == strcmp( argv[2],"air780" ) )
	{
		wdat->storage->net_mode = NetMode_Air780;
	}
	else
	{
		wdat->storage->net_mode = NetMode_Not;
	}
	PRT_LOG("config net %s \r\n",argv[2],val );
	save_data();
	return 0;

//------------------------------------------------------------------	

//------------------------------------------------------------------	
		
}
cmd_util_t __cmd_config={
	.name	 	= "config",
	.usage 	= "config",
	.text 	= "->input config to cat usage",
	.app 		= task_config,
};

/*-------------------------------------------------------------------------*/
static int task_cat_data( char argc, char **argv )
{
	if( argc >2 ) return -1;
	PRT_LOG("boot_code -->%d \r\n",wdat->storage->boot_code );
	PRT_LOG("program -->%d \r\n",wdat->storage->boot_app );
	PRT_LOG("net_mode -->%d \r\n",wdat->storage->net_mode );
	PRT_LOG("net_ptf -->%d \r\n",wdat->storage->net_ptf );

	PRT_LOG("y0 = %f *x0 + (%f) \r\n",wdat->storage->k[0],wdat->storage->b[0] );
	PRT_LOG("y1 = %f *x1 + (%f) \r\n",wdat->storage->k[1],wdat->storage->b[1] );
	
	

	return 0;
}
cmd_util_t __cmd_cat_data={
	.name	 	= "cat_data",
	.usage 	= "cat_data",
	.text 	= "->cat_data zd_val | curr_ratio0 | curr_ratio1 ",
	.app 		= task_cat_data,
};
/*-------------------------------------------------------------------------*/
static int task_ctrl( char argc, char **argv )
{
//	uint32_t sock = 0;
//	uint32_t pwr = 0;
//	time_t time = 0;
//	char *ddh="0000000000000000";
//	check_argc_error( 4,__usage_cmd );
//	
//	sock = atol( argv[1] )+1;	//0 1 在控制程序方对应 1 2
//	
//	if( 0 == strcmp( argv[2],"open" ) )
//	{	
//		pwr = __ON;
//		time = atol( argv[3] )*1000*60;
//	
//	}
//	else
//	{	
//		pwr = __OFF;
//		time = 0;
//	}
//	
//	//产生充电订单
//	sock_mq->send_order( ddh, sock, oct_start_charge, time );
//	
	PRT_LOG("this is bootloader,please use app!\r\n" );
	return 0;

	
}
cmd_util_t __cmd_ctrl={
	.name	 	= "ctrl",
	.usage 	= "ctrl <sock> <open/close> <time>",
	.text 	= "-- ",
	.app 		= task_ctrl,
};
/*-------------------------------------------------------------------------*/
static int task_ota( char argc, char **argv )
{
	PRT_LOG("user req ota\r\n" );

	wdat->storage->boot_app = _app_update;
	work_data_write();
	cmdline_man->quit_wait();
	return 0;
}
cmd_util_t __cmd_ota={
	.name	 	= "ota",
	.usage 	= "ota",
	.text 	= "->ota app1 ",
	.app 		= task_ota,
};
/*-------------------------------------------------------------------------*/
static int task_set_boot( char argc, char **argv )
{
	if( argc != 2 )	goto __usage_set_boot;
	
	if( 0 == strcmp( argv[1],"ota" ) )
	{
		wdat->storage->boot_app = _app_update;
		work_data_write();
		cmdline_man->quit_wait();
	}
	else if( 0 == strcmp( argv[1],"app1" ) )
	{
		wdat->storage->boot_app = _app_new;
		wdat->storage->rp_ota_first_boot = 1;						//说明本次开机要报告给服务器
		work_data_write();
	}
	else 
		goto __usage_set_boot;
	PRT_LOG("set_boot: %s -->OK\r\n",argv[1] );
	return 0;
	
__usage_set_boot:
	PRT_LOG("->set_boot <app>\r\n");
	PRT_LOG("\t<app>: ota / app1\r\n");
	
	return 0;
}
cmd_util_t __cmd_set_boot={
	.name	 	= "set_boot",
	.usage 	= "set_boot <app>",
	.text 	= "->set_boot <ota/app1> ",
	.app 		= task_set_boot,
};

/*-------------------------------------------------------------------------*/


//注册扩展调试命令
void register_debug_cmd(void)
{
	cmdline_man->regiser_one( &__cmd_config );
	cmdline_man->regiser_one( &__cmd_cat_data );
	cmdline_man->regiser_one( &__cmd_ctrl );
	cmdline_man->regiser_one( &__cmd_ota );
	cmdline_man->regiser_one( &__cmd_set_boot );
	
}
