
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	
//static uint8_t send_buf[512];

#define MaxArgc_Normal  10		//常规最大输入参数
#define MaxArgc_Time		30		//时间最大输入参数
#define PRT_FuncName( str ) { PRT_LOG("call %s:%s\r\n",__FUNCTION__,str); }
//-------------------------------------------------------------------------
static int deal_message_logon( uint8_t* dat )
{
	PRT_FuncName( dat );
	//接收回调，按理说不需要处理这个东西
	
	return 0;
}
//-------------------------------------------------------------------------
//处理服务器返回relogin
//赵工要求加这个命令，收到就需要重新连接服务器。
static int deal_message_relogin( uint8_t* dat )
{
	PRT_FuncName( dat );
	//接收回调
	
	int result = 0,i=0;
	char* cmd_line = (char*)dat;
	uint8_t argc;
	char * argv[ MaxArgc_Normal ];
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	for( int i=0;i<argc;i++ )	{PRT_LOG( "argv[%d] = %s\r\n",i,argv[i] );}
	
	char* token = "relogin";	
	result = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( result == 0  )
	{
		PRT_LOG( "err: token \r\n" );
		return -1;
	}
	
	//需要重新获取分时时段
	guide_dat->time_slot->get_slot = 0;
	
	return ret_ReConnect;	//重新连接
}


//-------------------------------------------------------------------------
//处理服务器返回nologin
//本质上，返回这个东西，说明客户端被服务器踢掉了。需要开启TCP，连接服务器。
static int deal_message_nologin( uint8_t* dat )
{
	PRT_FuncName( dat );
	//接收回调，按理说不需要处理这个东西
	int result = 0,i=0;
	char* cmd_line = (char*)dat;
	uint8_t argc;
	char * argv[ MaxArgc_Normal ];
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	for( int i=0;i<argc;i++ )	{PRT_LOG( "argv[%d] = %s\r\n",i,argv[i] );}
	
	char* token = "nologin";	
	result = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( result == 0  )
	{
		PRT_LOG( "err: token \r\n" );
		return -1;
	}
	
	return ret_ReConnect;	//重新连接
}
//-------------------------------------------------------------------------
static int deal_message_over( uint8_t* dat )
{
	PRT_FuncName( dat );

	char ddh[MaxDDH_Len];
	pool_pt pool;
	
	int result = 0,i=0;
	char* cmd_line = (char*)dat;
	uint8_t argc;
	char * argv[ MaxArgc_Normal ];
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	for( int i=0;i<argc;i++ )	{PRT_LOG( "argv[%d] = %s\r\n",i,argv[i] );}
	
	char* token = "over";	
	result = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( result == 0  )
	{
		PRT_LOG( "err: token \r\n" );
		return -1;
	}
	
	//抽出ddh
	for( i=0;i<MaxDDH_Len;i++ ) ddh[i] = 0;
	for( i=0;i<strlen( argv[1] );i++ ) ddh[i] = argv[1][i];
	
	
	pool = ipool->get_by_ddh( ddh );
	if( pool != NULL )
	{
		PRT_LOG("\tdelate_one pool,ddh = %s \r\n",pool->event.ddh );
		ipool->delate_one	( pool );	//从交互池中删除
		ipool->destroy		( pool ); //彻底从内存中释放
	}
	else
	{
		PRT_LOG("can't find ddh = %s \r\n",pool->event.ddh );
	}
	
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_log( uint8_t* dat )
{
	PRT_FuncName( dat );
	//接收回调，按理说不需要处理这个东西
	
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_pong( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_PARAM( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_STATU( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	return 0;
}

//-------------------------------------------------------------------------
//尖峰平谷 分时时段
static uint32_t check_sum_str(char* str)
{
	int i = 0;
	uint32_t sum = 0;
	for (i = 0; str[i] != '\0'; i++) {
			// 检查字符是否为数字（'0'到'9'的范围）
			if (str[i] >= '0' && str[i] <= '9') {
					// 将字符转换为数字并累加
					sum += str[i] - '0'; // '0'的ASCII值为48，减去它将字符转换为对应的整数
			}
	}
	return sum;
}
//jfpg 10:30-12:00 12:00-14:00,18:00-19:00 14:00-16:00 16:00-18:00,23:00-24:00,00:01-01:00 75
extern  void time_slot_dump(int group,char* str);

static int deal_message_jfpg( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	int result = 0;

	char* cmd_line = (char*)dat;
	uint8_t argc;
	char * argv[ MaxArgc_Normal ];
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	for( int i=0;i<argc;i++ )	{PRT_LOG( "argv[%d] = %s\r\n",i,argv[i] );}
	
	char* token = "jfpg";	
	result = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( result == 0  )
	{
		PRT_LOG( "err: token \r\n" );
		return -1;
	}
	
	
	//判断是否合法
	uint32_t recv_check_sum = atol( argv[argc-1] );
	uint32_t my_check_sum = 0; 
	for( int i=0;i<(argc-1);i++ )
	{
		my_check_sum += check_sum_str( argv[i] );
	}
	PRT_LOG("my_check_sum = %d \r\n",my_check_sum);
	if( my_check_sum != recv_check_sum ) return -2;
	
	PRT_LOG("dump jfpg\r\n");
	
	//逐个时段拆分
	char* time_argv[ MaxArgc_Normal ];
	uint8_t time_argc = 0;
	
	guide_dat->time_slot->segments = 0;			//
	guide_dat->time_slot->groups = argc - 2;
	for( int group=1;group<(argc-1);group++ )
	{
		cmd_line = argv[group];
		time_argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, time_argv,",\t\n\r");
		PRT_LOG("Group[%d]:\r\n",group-1);
		for( int i=0;i<time_argc;i++ )	
		{
			time_slot_dump( group-1,time_argv[i] );	//group从0开始填充
			PRT_LOG( "\t time_argv[%d] = %s\r\n",i,time_argv[i] );
		}
		
	}
	
	
	guide_dat->time_slot->get_slot = 1;		//分时时段获取成功
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_time( uint8_t* dat )
{
	PRT_FuncName( dat );

	int result = 0;

	char* cmd_line = (char*)dat;
	uint8_t argc;
	char * argv[ MaxArgc_Normal ];
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	for( int i=0;i<argc;i++ )	{PRT_LOG( "argv[%d] = %s\r\n",i,argv[i] );}
	
	char* token = "time";	
	result = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( result == 0  )
	{
		PRT_LOG( "err: token \r\n" );
		return -1;
	}
	
	//判断是否合法
	uint32_t recv_check_sum = atol( argv[argc-1] );
	uint32_t my_check_sum = 0; 
	for( int i=0;i<(argc-1);i++ )
	{
		my_check_sum += check_sum_str( argv[i] );
	}
	PRT_LOG("my_check_sum = %d \r\n",my_check_sum);
	if( my_check_sum != recv_check_sum ) return -2;
	
	//填充日期
	dump_rtc_by_str( argv[1] );
	
	
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_setparam( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_getstatu( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	return 0;
}
//-------------------------------------------------------------------------
static int deal_message_update( uint8_t* dat )
{
	PRT_FuncName( dat );
	char ddh[MaxDDH_Len];
	
	unsigned char argc;
	char * argv[ MaxArgc_Normal ];
	char* cmd_line = (char*)dat;
	int i = 0;

	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	for( int i=0;i<argc;i++ )	{sysprt->alog( "argv[%d] = %s\r\n",i,argv[i] );}
	
	char* token = "update";	
	int ret = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( ret == 0  )
	{
		sysprt->alog( "err: token \r\n" );
		return -1;
	}
	
	{//此处先生产一个维护记录给服务器，在维护记录发送程序中，进入OTA模式
		
		PRT_LOG("req ota\r\n" );
		for( i=0;i<MaxDDH_Len;i++ ) 	{ddh[i] = 0;	}
		sock_mq->put_event( ddh,_event_m_tenance,0x00,0x00,0,0,PNULL);
	}
	return 0;
}

//-------------------------------------------------------------------------
//下单命令
/*
	工作流程，下单时，根据当前时间+订单时长，计算得到结束时间。
	并把此时的表底信息存储到flash中。
	
*/
static int deal_message_on( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	unsigned char argc;
	char * argv[ MaxArgc_Normal ];
	char* cmd_line = (char*)dat;
	int i = 0;
	
	uint8_t com = 0;			//服务器角度，插座id
	uint8_t id = 0;				//硬件角度，插座id
	uint32_t time = 0;		//时长
	uint32_t len = 0;			//临时，长度
	event_t event;
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	for( int i=0;i<argc;i++ )	{sysprt->alog( "argv[%d] = %s\r\n",i,argv[i] );}
	
	char* token = "on";	
	int ret = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( ret == 0  )
	{
		sysprt->alog( "err: token \r\n" );
		return -1;
	}

	//订单信息
	com = atol( argv[1] );							//获取插座id
	if( com >= (MaxSock+1) ) return -1;	//服务器描述插座id从1开始数
	id = com - 1;
	
	time = atol( argv[2] );							//获取时长
	if( time == 0 ) time = 10*60;				//单位 分钟
	else if( time > (12*60) )						//限制一下服务器不能发太长时间
	{
		time = 10*60;
	}
	
	
	len = strlen( argv[3] );						//获取订单号长度
	if( len>=30 ) len =30;
	//订单号
	for( i=0;i<MaxDDH_Len;i++ ) 	{event.ddh[i] = 0;	}
	for( i=0;i<len;i++ )					{	event.ddh[i] = argv[3][i];	}
	
	sysprt->alog( "DDH ->%s \r\n",event.ddh );
	//回一条电d流消息给服务器,免得服务器一直发
	event.num = id;								//插座id ，硬件上的插座id
	event.val = 0;								//不使用
	event.dat = 1;								//报文序号 固定1
	event.curr = 0;								//平均电流 0
	event_report_curr( &event );	//把电流报文 发送出去 
	//此处不修改 		guide_dat->rtd_num->sock[road]，将订单分发给sock_ctrl，由sock_ctrl处理
	
	//把订单消息发给sock_ctrl
	sock_mq->send_order( event.ddh,id,oct_start_charge,time,0 );
		

		
	return 0;
}

static int deal_message_off( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	unsigned char argc;
	char * argv[ MaxArgc_Normal ];
	char* cmd_line = (char*)dat;
	int i = 0;
	
	uint8_t com = 0;			//服务器角度，插座id
	uint8_t id = 0;				//硬件角度，插座id
	uint32_t time = 0;		//时长
	uint32_t len = 0;			//临时，长度

	
	char ddh[MaxDDH_Len];
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	char* token = "off";	
	int ret = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( ret == 0  )
	{
		sysprt->alog( "err: token \r\n" );
		return -1;
	}
	
	com = atol( argv[1] );							//获取插座id
	if( com >= (MaxSock+1) ) return -1;	//服务器描述插座id从1开始数
	id = com - 1;
	
	len = strlen( argv[2] );						//获取订单号长度
	if( len>=30 ) len =30;
	for( i=0;i<MaxDDH_Len;i++ ) 	{ddh[i] = 0;	}
	for( i=0;i<MaxDDH_Len;i++ ) 	{ddh[i] = argv[2][i];	}
	
	//把订单消息发给sock_ctrl
	sock_mq->send_order( ddh,id,oct_cancel_charge,0,0 );
	
	return 0;
}

static int deal_message_reboot( uint8_t* dat )
{
	PRT_FuncName( dat );
	
	unsigned char argc;
	char * argv[ MaxArgc_Normal ];
	char* cmd_line = (char*)dat;
	int i = 0;
	
	uint8_t com = 0;			//服务器角度，插座id
	uint8_t id = 0;				//硬件角度，插座id
	uint32_t time = 0;		//时长
	uint32_t len = 0;			//临时，长度

	
	char ddh[MaxDDH_Len];
	
	argc = strlib->split_string_n( MaxArgc_Normal,cmd_line, argv," \t\n\r");
	if (argc == 0) return 0;
	
	char* token = "reboot";	
	int ret = strlib->strcmpn( token,argv[0],strlen( token ) );
	if( ret == 0  )
	{
		sysprt->alog( "err: token \r\n" );
		return -1;
	}
	
	reset_system();
	
	return 0;
}
//=========================================================================

deal_srv_cmd_t wira_deal_cmd[_srv_cmd_max] = {
	deal_message_logon,							
	deal_message_relogin,
	deal_message_nologin,
	deal_message_over,
	deal_message_log,
	deal_message_pong,
	deal_message_PARAM,
	deal_message_STATU,
	deal_message_jfpg,
	deal_message_time,
	deal_message_setparam,
	deal_message_getstatu,
	deal_message_update,
	deal_message_on,
	deal_message_off,
	deal_message_reboot,
	
};

//如果实在搞不定，有些命令码可以加一个空格，避免包含识别，或者调整先后顺序
char* _srv_cmd_list[_srv_cmd_max]={
	"logon",
	"relogin",
	"nologin",
	"over",
	"log",
	"pong",
	"PARAM",
	"STATU",
	"jfpg",
	"time",
	"setparam",
	"getstatu",
	"update",
	"on",
	"off",
	"reboot",
	
};

//-------------------------------------------------------------------------
