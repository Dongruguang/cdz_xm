#include "work.h"
#include "network.h"
#include "message.h"

/*-------------------------------------------------------------------------------------*/

char* get_vision( void );
/*-------------------------------------------------------------------------------------*/

wk_net_t wk_net={0,0,0,0};

static net_dev_t m_net_dev;

net_dev_t* wk_netdev = &m_net_dev;


/*-------------------------------------------------------------------------------------*/

enum{
	_e_port_init = 0,				//接口初始化
	_e_rst_net_dev ,				//复位网络设备
	_e_init_net_dev,				//初始化网络设备
	_e_connect_serv,				//连接服务器
	
};




void network_entry( void )
{
	int val = 0;
	
	switch( wk_net.msta )
	{
		case _e_port_init:
			NET_LINE();
			//指定通讯串口
			air_opt->create_dev( wk_netdev,uart1 );
			//设置  轮序作业  回调函数
			air_opt->install_net_cycle( net_cycle );	
			//设置  消息循环  回调函数
			air_opt->install_msg_cycle( msg_cycle );
			wk_net.msta++;
			break;
		
		case _e_rst_net_dev:
			val = air_opt->work_rst_dev( wk_netdev );
			if( val == 0 ) break;
			air_opt->clr_sta( wk_netdev );
			wk_net.msta++;
			break;
		
		case _e_init_net_dev:
			val = air_opt->work_init_dev( wk_netdev );
			if( val == 0 ) break;
			air_opt->clr_sta( wk_netdev );
			if( val == -1 ) 
			{//发生了不可预料的错误，直接重启网络
				wk_net.msta=_e_rst_net_dev;
				wk_net.ssta=0;
			}
			else
			{//没有错误，则继续下一步
				wk_net.msta++;
				wk_net.ssta=0;
			}
			
			break;
		

		
		case _e_connect_serv:
			val = air_opt->work_connect_serv( wk_netdev,
				wdat->storage->user_name,wdat->storage->user_pwd,get_vision()
			);
			if( val == 0 ) break;
			air_opt->clr_sta( wk_netdev );
			if( val == -1 ) 
			{//发生了不可预料的错误，直接重启网络
				wk_net.msta=_e_rst_net_dev;
				wk_net.ssta=0;
			}
			
			break;
		
		
		default:
			break;
	}
	
	
}
/*-------------------------------------------------------------------------------------*/

static char vision[12];
static int vflag = 0;
char* get_vision( void )
{
	if( vflag == 0 )
	{//填充字符串
		snprintf(vision,sizeof(vision),"%d",App_Version*10+App_SubVersion );
		vflag = 1;
	}
	
	return vision;
	
}
/*-------------------------------------------------------------------------------------*/
