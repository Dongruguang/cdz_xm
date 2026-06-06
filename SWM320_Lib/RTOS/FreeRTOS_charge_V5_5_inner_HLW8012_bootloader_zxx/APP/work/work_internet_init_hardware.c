
#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)	

static void air724_init_hardware( wk_inet_pt inet );
//------------------------------------------------------------------------------

void work_internet_init_hardware( wk_inet_pt inet )
{
//	static time_t otick=0,ntick=0;
	switch( inet->ssta )
	{
		case 0:
			air724_init_hardware( inet );
		
			break;
		
		default:
			break;
	}
	
}
//------------------------------------------------------------------------------


#define CMD_MaxErr	3
#define __ERR_AIR_RST( ) 	{ line = __LINE__;goto __AIR_RST; }

static void air724_init_hardware( wk_inet_pt inet )
{
	
//	char* argv[5];

	int err = 0,ret = 0,line = 0;

		
	air_dev_pt  this_dev;
	this_dev = air724;
	
		
	//-----------------------------------------------------------------
	if( wdat->storage->net_mode == NetMode_Air724 )
	{//Air724支持自动匹配波特率
		//匹配波特率
		air724->auto_baudrate( this_dev,NULL,NULL );
		//匹配的时候，发送了很多AT给设备，要发送 "AT\r\n" 让这条命令结束
		while( 1 )
		{		
			ret = air724->cmd_AT( this_dev,NULL,NULL );
			air_man->ptf_rxbuf( this_dev,NULL );
			if( ret == 1 ) break;
		}
	}
	
	//读取模块厂商信息
	air724->cmd_AT_CGMI( this_dev,NULL,NULL );
	air_man->ptf_rxbuf( this_dev,NULL );
	
	//读取固件版本信息
	air724->cmd_AT_CGMR( this_dev,NULL,NULL );
	air_man->ptf_rxbuf( this_dev,NULL );
	
	//查询卡是否插好
	err = 0;
	while( 1 )
	{
		ret = air724->cmd_AT_CPIN( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(500);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	
	//查询设置信号质量
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CSQ( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(1000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	
	//查询网络注册状态
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CREG( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(2000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	
	//查询附着GPRS网络
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CGATT( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(2000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	//设置透传模式
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CIPMODE( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(2000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	
	//设置IP为单链接
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CIPMUX( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(2000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	//设置自动获取APN
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CSTT( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(2000);
		if( (err++)>CMD_MaxErr ){__ERR_AIR_RST();}
	}
	
	
	//激活移动场景，激活后能获取到IP
	err = 0;
	while( 1 )
	{		 
		ret = air724->cmd_AT_CIICR( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		thread_delay_ms(2000);
		if( (err++)>1 ){__ERR_AIR_RST();}
	}
	
	//查询IP，只有获取到IP后才能上网
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CIFSR( this_dev,NULL,NULL );
		air_man->ptf_rxbuf( this_dev,NULL );
		if( ret == 1 ) break;
		task_delay(2000);
		if( (err++)>1 ){__ERR_AIR_RST();}
	}
	
	
	inet->msta++;
	inet->ssta = 0;
	return ;
//	air_man->send_str( this_dev,"+++" );
__AIR_RST:
	sysprt->aerr("\r\n%s (%d) \r\n",__FILE__,line);
	//请求复位
	inet->msta = 0;
	inet->ssta = 1;
	return;
}






