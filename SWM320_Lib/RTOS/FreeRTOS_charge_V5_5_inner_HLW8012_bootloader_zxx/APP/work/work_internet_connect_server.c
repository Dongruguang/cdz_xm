
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	

static int air724_connect_server( wk_inet_pt inet );
static void login_in( void );
static void ping( void );
static int wait_logon( wk_inet_pt inet );

static uint8_t* rxbuf = NULL;
static uint32_t rd_len = 0;											//保存临时读到的数据尺寸 read len

//------------------------------------------------------------------------------

void work_internet_connect_server( wk_inet_pt inet )
{
	int ret = 0;
	static time_t otick=0,ntick=0;
	
	switch( inet->ssta )
	{
		case 0://
			inet->ssta ++;
		
			break;
		case 1://连接+login
			
			air724->rxlen = 0;
			rxbuf = air724->rxbuf;								
			memset( rxbuf,0,1024 ); 
			ret = air724_connect_server( inet );	
			
			if( ret < 0 ) break;
		
			//登录
			login_in();
			otick = get_sys_ticks();						//获取起始tick
			
			air724->rxlen = 0;
			rxbuf = air724->rxbuf;								
			memset( rxbuf,0,1024 ); 
			inet->ssta ++;
		
			break;
		
		case 2://等LOGON
			
			ret = wait_logon( inet );
			if( ret >= 0 ) break;
			//超时处理
			ntick = get_sys_ticks();
			if( 5000>( ntick - otick ) ) break;
			//重启网络服务
			inet->msta = 0;
			inet->ssta = 1;
		
			break;
		
		case 0xf0:
			break;
		
		default:
			break;
	}
	
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------



#define CMD_MaxErr	5
#define __ERR_AIR_RST( ) 	{ line = __LINE__;goto __AIR_RST; }

static int air724_connect_server( wk_inet_pt inet )
{
	
		char* argv[5];

	int err = 0,ret = 0,line = 0;

		
	air_dev_pt  this_dev;
	this_dev = air724;
	//-------------------------------------------
	while( 1 )
	{		
		ret = air724->cmd_AT( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
	}
	
	
	//-------------------------------------------
	//创建TCP连接
	argv[0] = "TCP";							// TCP/UDP
	//旧服务器
//	argv[1] = "14.116.153.234";		// IP addr
	//新的充电桩专用服务器
	argv[1] = "14.18.45.140";		// IP addr
	argv[2] = "9000";							// port
	
	while( 1 )
	{		
		ret = air724->cmd_AT_CIPSTART( this_dev,3,argv );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		task_delay(2000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	sysprt->aerr("connect tcp success \r\n");
	return 1;
//	air_man->send_str( this_dev,"+++" );
__AIR_RST:
	sysprt->aerr("\r\n%s (%d) \r\n",__FILE__,line);
	//请求复位
	inet->msta = 0;
	inet->ssta = 1;
	
	return -1;
}
//------------------------------------------------------------------------------

extern uint8_t* get_internet_encode_out(void);
static uint8_t  send_buf[512];

//char* sn = "302";

static void login_in( void )
{		
	air_dev_pt  this_dev;
	this_dev = air724;
	memset( send_buf,0,sizeof( send_buf ) );
	int dump_len = 0;
	int ret = 0;
	
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"LOGIN " );
	dump_len += ret;
	
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),get_msh_terminal_str() );
	dump_len += ret;
	
	ret = strlib->dump( (char*)(&send_buf[ dump_len ])," " );
	dump_len += ret;
	
	
	
	
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),(char*)get_internet_encode_out() );
	dump_len += ret;
	
	ret = strlib->dump( (char*)(&send_buf[ dump_len ]),"\r\n" );
	dump_len += ret;

	send_buf[ dump_len ] = '\0';
	
	sysprt->alog( "send:%s",send_buf );
	sysprt->alog( "len :%d\r\n",strlen( (char*) send_buf) );
	
	//发送
	air_man->send_str( this_dev, (char*)send_buf );

	
}

//------------------------------------------------------------------------------
//
static int wait_logon( wk_inet_pt inet )
{
//	int err = 0;
//	air_dev_pt  this_dev;
//	this_dev = air724;
	int ret = 0;
	//接收数据
	rd_len = air724->uart->read_recv_buf( &(rxbuf[ air724->rxlen ]),1 );
	//如果没有，就直接退出
	if( rd_len <= 0 ) return -1 ;
	
	air724->rxlen += rd_len;
	
	ret = strlib->findstr( (char*)rxbuf,"\r\n" );
	
	//如果没有收到回车符号，直接退出
	if( ret < 0 ) return -2;
	
	air_man->ptf_rxbuf( air724,NULL );
	
	
	ret = strlib->findstr( (char*)rxbuf,"Logon" );
	if( ret >= 0 ) 
	{
		sysprt->alog( "LOGON->>>>work cycle\r\n" );
		
		goto __next_msta;
	}
	
	
	
	
	goto __invalid_data;
	
__invalid_data:
	memset( rxbuf,0,1024 );	
	rxbuf = air724->rxbuf;
	air724->rxlen = 0;
	
	return 0;
__at_invalid_cmd_data:
	
	return 1;
__next_msta:
	led_board->set_net_led ( __ON );
	inet->msta++;
	inet->ssta = 0;
	
	return 1;
}


