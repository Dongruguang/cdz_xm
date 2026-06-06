#include "message.h"
#include <string.h>


//获取分时时段
static void get_slot( void* ndev )
{
	static uint32_t otick_slot = 0;
	uint32_t ntick =0;

	if( guide_dat->time_slot->get_slot != 0 ) return;
	
	ntick = get_sys_ticks();
	if( ( ntick - otick_slot ) <= 5000 ) return;
	otick_slot = ntick;
	
	net_dev_t* netdev = (net_dev_t*)ndev;
	//定时发送
//	air_opt->send_str( (net_dev_t*)ndev, "GETJFPG\r\n" );	
	char* tbuf = netdev->txbuf;
	snprintf( tbuf,Ndev_TX_BUF_LEN,"GETJFPG\r\n" );
	air_opt->send_str( (net_dev_t*)ndev, tbuf );	
	show_send_data( tbuf );
}
/*-------------------------------------------------------------------------------------*/
//定时同步网络时间
static void get_net_time( void* ndev,int wait )
{
	static uint32_t otick_slot = 0;
	uint32_t ntick =0;
	
	net_dev_t* netdev = (net_dev_t*)ndev;
	char* tbuf = netdev->txbuf;
	
	if( 0 == wait ) goto __get_time;
	
	ntick = get_sys_ticks();
	if( ( ntick - otick_slot ) <= (10*60*1000) ) return;
	otick_slot = ntick;

__get_time:	
	//定时发送
	snprintf( tbuf,Ndev_TX_BUF_LEN,"GETTIME\r\n" );
	air_opt->send_str( (net_dev_t*)ndev, tbuf );	
	show_send_data( tbuf );
	return;
}
/*-------------------------------------------------------------------------------------*/

static void ping_serv( void* ndev,int wait )
{
	static uint32_t otick_slot = 0;
	uint32_t ntick =0;
	
	net_dev_t* netdev = (net_dev_t*)ndev;
	char* tbuf = netdev->txbuf;
	
	if( 0 == wait ) goto __ping;
	
	ntick = get_sys_ticks();
	if( ( ntick - otick_slot ) <= (10*1000) ) return;
	otick_slot = ntick;
__ping:	
	//定时发送
	
	snprintf( tbuf,Ndev_TX_BUF_LEN,"PING 0 0 220\r\n" );
	air_opt->send_str( (net_dev_t*)ndev, tbuf );	
	show_send_data( tbuf );
	return;
}

/*-------------------------------------------------------------------------------------*/

int net_cycle( void* ndev )
{
	net_dev_t* netdev = (net_dev_t*) ndev; 
	switch( netdev->ssta )
	{
		case 0:
			get_net_time( ndev,0 );
			netdev->ssta++;
			break;
		
		case 1:
			//获取分时时段
			get_slot( ndev );
			//定时同步网络时间
			get_net_time( ndev,1 );
			//定时发送PING消息
			ping_serv( ndev,1 );
			
			//硬件事件上报
			
			//交互事件处理
			ipool->work();
			break;
		
		default:
			break;
	}
	
	
	return 0;
}


int msg_cycle( void* ndev,int argc,char** argv )
{
	int fun_ret = 0;
	char* cmd_str = NULL;
	for( int index=0;index<_srv_cmd_max;index++ )
	{
		cmd_str = _srv_cmd_list[ index ];
		if( 0 == strncmp( argv[0],cmd_str,strlen( cmd_str ) ) )
		{
			//执行对应回调
			fun_ret = wira_deal_cmd[index]( ndev,argc,argv );
		}
	}
	
	return fun_ret;
}





