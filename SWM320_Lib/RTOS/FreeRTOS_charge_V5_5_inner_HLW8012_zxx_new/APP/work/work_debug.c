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
	uint32_t val = 0,i=0;
//	double ratio = 0; 
	float k=0,b=0;
	
	check_argc_error( 2,__usage_cmd );
	
	if( 0 == strcmp( argv[1],"kb" ) )
	{//直接配置,拟合直线系数 k和b
		check_argc_error( 4,__usage_kb );
		goto __deal_kb;
	}
	else if( 0 == strcmp( argv[1],"rst_kb" ) )
	{//直接配置,拟合直线系数 k和b
		check_argc_error( 2,__usage_rst_kb );
		goto __deal_rst_kb;
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
	else if( 0 == strcmp( argv[1],"ota_server" ) )
	{////配置上网方式
		check_argc_error( 3,__usage_ota_server );
		goto __deal_ota_server;
	}
	else if( 0 == strcmp( argv[1],"zd_user" ) )
	{////配置用户账号
		check_argc_error( 4,__usage_zd_user );
		goto __deal_zd_user;
	}

	
	return 0;
//------------------------------------------------------------------	
__usage_cmd:
	PRT_LOG("->config <cmd>\r\n");
	PRT_LOG("cmd:zd_val | ratio | hlw_ptf | net_ptf | store | default | net\r\n" );
	return -1;

//------------------------------------------------------------------	
__usage_kb:
	
	PRT_LOG("->config kb <sock> <kval> <bval>\r\n");
	PRT_LOG("\t<sock>: 0 / 1 \r\n");
	PRT_LOG("\t<val> : y = kx + b \r\n");
	
	return -2;
	
__deal_kb:
	val = atol( argv[2] );	//插座号
	if( val >= 2 )
	{
		PRT_LOG( "err: sock only support: 0~1\r\n" );
		goto __usage_kb;
		
	}
	k = atof( argv[3] );
	b = atof( argv[4] );
	
	
//	wdat->curr_ratio[ val ] = now_ratio/wdat->curr_ratio[ val ]/out_ratio;
	wdat->storage->k[ val ] = k;
	wdat->storage->b[ val ] = b;
	PRT_LOG("config kb[%d] -->y = %.5f + %.5f \r\n",val,wdat->storage->k[ val ],wdat->storage->b[ val ] );
	save_data();

	return 0;
//------------------------------------------------------------------	
__usage_rst_kb:
	PRT_LOG("->config rst_kb <sock>\r\n");
	PRT_LOG("\t<sock>: 0 / 1 \r\n");
	return -2;
	
__deal_rst_kb:
	val = atol( argv[2] );	//插座号
	if( val >= 2 )
	{
		PRT_LOG( "err: sock only support: 0~1\r\n" );
		goto __usage_rst_kb;
		
	}
	wdat->storage->k[ val ] = 1;
	wdat->storage->b[ val ] = 0;
	PRT_LOG("config rst_kb [%d] -->y = %.5f + %.5f  \r\n",val,wdat->storage->k[ val ],wdat->storage->b[ val ] );
	return 0;

//------------------------------------------------------------------	
		
__usage_store:
	
	return -2;
__deal_store:
	work_data_write();
	PRT_LOG("config store -->OK \r\n" );
	
	guide_dat->order_info->sock[0].working = 1;
	guide_dat->order_info->sock[0].end.Year = 2024;
	guide_dat->order_info->sock[0].end.Month = 11;
	guide_dat->order_info->sock[0].end.Date = 27;
	guide_dat->order_info->sock[0].end.Hour = 2;
	guide_dat->order_info->sock[0].end.Minute = 10;
	guide_dat->order_info->sock[0].end.Second = 2;
	
	wdat_opt->order_info.set_to_fdb( guide_dat->order_info );
	
	
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

	PRT_LOG("config resrt -->OK \r\n",val );
	save_data();
	reset_system();
	while(1);
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
	PRT_LOG("config net %s-->OK \r\n",argv[2],val );
	save_data();
	return 0;
	
//------------------------------------------------------------------	
__usage_ota_server:
	PRT_LOG("->config ota_server <serv>\r\n");
	PRT_LOG("\t<serv>: ota_server/test_server \r\n");
	return -2;
__deal_ota_server:
	
	
	PRT_LOG("config ota_server %s-->OK \r\n",argv[2] );
	save_data();
	return 0;

//------------------------------------------------------------------	
__usage_zd_user:
	PRT_LOG("->config user <name> <password>\r\n");
	return -2;
__deal_zd_user:
	memset( wdat->storage->user_pwd,0,MaxLen_User );
	
	//用户名
	val = strlen( argv[2] );
	if( val < (MaxLen_User-1) )
	{//最大只能存 (MaxLen_User-1) 长度
		memset( wdat->storage->user_name,0,MaxLen_User );
		for( i=0;i<val;i++ )
		{
			wdat->storage->user_name[i] = argv[2][i];
		}
	}
	else
	{
		PRT_LOG("Err: user_name is too long\r\n");
		return -1;
	}

	//密码
	val = strlen( argv[3] );
	if( val < (MaxLen_User-1) )
	{//最大只能存 (MaxLen_User-1) 长度
		memset( wdat->storage->user_pwd,0,MaxLen_User );
		for( i=0;i<val;i++ )
		{
			wdat->storage->user_pwd[i] = argv[3][i];
		}
	}
	else
	{
		PRT_LOG("Err: user_pwd is too long\r\n");
		return -1;
	}
	
	save_data();
	PRT_LOG("user set ->OK\r\n");
	return 0;

//------------------------------------------------------------------	
		
}
cmd_util_t __cmd_config={
	.name	 	= "config",
	.usage 	= "config",
	.text 	= "->input config to cat usage",
	.app 		= task_config,
};

/*-------------------------------------------------------------------------*/

void prt_date_time( rtc_dt_t* dt,char* str )
{
	PRT_LOG("%s:%04d/%02d/%02d-%02d:%02d:%02d\r\n",str,dt->Year,dt->Month,dt->Date,   dt->Hour,dt->Minute,dt->Second);
		
}


static int task_cat_data( char argc, char **argv )
{
//	uint32_t val = 0;
//	double ratio = 0; 
	if( argc >2 ) return -1;

	
	PRT_LOG("boot_code -->%d \r\n",wdat->storage->boot_code );
	PRT_LOG("boot_app  -->%d \r\n",wdat->storage->boot_app );
	PRT_LOG("net_mode  -->%d \r\n",wdat->storage->net_mode );
	PRT_LOG("net_ptf -->%d \r\n",wdat->storage->net_ptf );


	PRT_LOG("y0 = %f *x0 + (%f) \r\n",wdat->storage->k[0],wdat->storage->b[0] );
	PRT_LOG("y1 = %f *x1 + (%f) \r\n",wdat->storage->k[1],wdat->storage->b[1] );
	
	

	rtc_dt_t *dt = 0;
	//打印订单情况
	for( int id=0;id<MaxSock;id++ )
	{
		PRT_LOG("\r\nSock[%d]\r\n",id );
		
		
		PRT_LOG("working:%d\r\n",guide_dat->order_info->sock[id].working );
		PRT_LOG("ddh    :%s\r\n",guide_dat->order_info->sock[id].ddh );
		
		dt = &(guide_dat->order_info->sock[id].start);
		prt_date_time(dt,"start  ");
		dt = &(guide_dat->order_info->sock[id].end);
		prt_date_time(dt,"end    ");
		
		PRT_LOG("\r\n\r\n");

		
	}
	
	
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
	uint32_t sock = 0;
//	uint32_t pwr = 0;
	time_t time = 0;
	int cmd = 0;
	
	char *ddh="0000000000000000";
	check_argc_error( 4,__usage_cmd );
	
	sock = atol( argv[1] );	//0 1 
	
	if( 0 == strcmp( argv[2],"open" ) )
	{	
//		pwr = __ON;
		time = atol( argv[3] )*1000*60;
		cmd = oct_start_charge;
	}
	else
	{	
//		pwr = __OFF;
		time = 0;
		cmd = oct_cancel_charge;
	}
	
	//产生充电订单
	sock_mq->send_order( ddh, sock, cmd, time,0 );
	
	
	PRT_LOG("ctrl ->sock[%d] %s ->%d min \r\n",sock,argv[2],time );
	return 0;

__usage_cmd:
	PRT_LOG("ctrl ->sock[%d] %s ->%d min \r\n",sock,argv[2],time );
	return -1;
	
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
	req_goto_update();
	
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
	PRT_LOG("\terr:please go to boot loader set boot\r\n");
	
	
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
