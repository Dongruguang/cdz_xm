#include "work.h"
#define PRT_LOG(X...)	sysprt->alog(X)		


static uint8_t* rxbuf = NULL;
static uint32_t rd_len = 0;											//保存临时读到的数据尺寸 read len




void wira_work_data_rst( void )
{
	//接收缓冲区复位
			
	rxbuf = air724->rxbuf;
	memset( rxbuf,0,1024 );
	
	air724->rxlen = 0;
	
}

int wira_wrok_cycle( wk_inet_pt inet )
{
	int err = 0;
	int ret = 0;
	int cmd_ret = 0;
	//接收数据
	rd_len = air724->uart->read_recv_buf( &(rxbuf[ air724->rxlen ]),1 );
	//如果没有，就直接退出
	if( rd_len <= 0 ) return -1 ;
	
	air724->rxlen += rd_len;

	//如果长度超过512,舍弃数据,重新接收
	if( air724->rxlen >= 512 )
	{
		goto __invalid_data;
	}
	
	ret = strlib->findstr( (char*)rxbuf,"\r\n" );
	
	
	//如果没有收到回车符号，直接退出
	if( ret < 0 ) return -2;
	
	
	
	//分析是什么数据
/*
	三种类型处理方案
	(1)AT指令数据，分情况讨论，但是也要复位接收，并返回
	(2)应用协议数据，按协议文档处理。
	(3)无效数据，直接复位接收，并返回0
	
*/
	air_man->ptf_rxbuf( air724,NULL );
	
	//(1)AT模块的消息
	ret = strlib->findstr( (char*)rxbuf,"CLOSED" );
	if( ret >= 0 ) 
	{
		PRT_LOG( "recv CLOSED \r\n" );
		cmd_ret = ret_ReConnect;
		goto __at_cmd_data;
	}
	ret = strlib->findstr( (char*)rxbuf,"PING" );
	if( ret >= 0 ) 
	{
		PRT_LOG( "recv PING \r\n" );

		PRT_LOG( "PING ,is error \r\n" );
		cmd_ret = ret_RstNet;

		goto __at_cmd_data;
	}
	
	//(2)处理服务器发过来的数据
	for( int index=0;index<_srv_cmd_max;index++ )
	{
		ret = strlib->findstr( (char*)rxbuf, _srv_cmd_list[ index ] );
		if( ret >= 0 ) 
		{
			//执行对应回调
			cmd_ret = wira_deal_cmd[index]( rxbuf );
		
			if( ret_RstNet == cmd_ret )
			{//重启网络
				goto __at_cmd_data;
			}
			else if( ret_ReConnect == cmd_ret )
			{//重新连接
				goto __at_cmd_data;
			}
			goto _protocol_data;
		}
	
	}
	
	goto __invalid_data;
	
__invalid_data:
	memset( rxbuf,0,1024 );	
	rxbuf = air724->rxbuf;
	air724->rxlen = 0;
	
	return 0;
__at_cmd_data:
	memset( rxbuf,0,1024 );	
	rxbuf = air724->rxbuf;
	air724->rxlen = 0;

	//退出透传模式
	thread_delay_ms( 1500 );
	air_man->send_str( air724,"+++" );
	thread_delay_ms( 1000 );
	
	//关闭TCP连接
	err = 0;
	while( 1 )
	{		
		ret = air724->cmd_AT_CIPCLOSE( air724,NULL,NULL );
		air_man->ptf_rxbuf( air724,NULL );
		if( ret >= 0 ) break;
		inet->ssta = 1;		
	}
	switch( cmd_ret )
	{
		case ret_ReConnect:
			PRT_LOG( "ret_ReConnect \r\n" );
			//重新连接服务器
			inet->msta = 3;
			inet->ssta = 0;
			break;
		
		default:
			PRT_LOG( "ret_RstNet \r\n" );
			//重启网络模块
			inet->msta = 0;
			inet->ssta = 1;
			break;
	}
	
	return -3;
	
	
_protocol_data:
	nwdt->feed( inet ); //喂狗
	
	
	memset( rxbuf,0,1024 );
	rxbuf = air724->rxbuf;
	air724->rxlen = 0;
	return 1;
	
}

